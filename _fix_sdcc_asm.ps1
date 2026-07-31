# _fix_sdcc_asm.ps1
# Post-processes SDCC-generated z80asm .asm files for z88dk-z80asm compatibility.
param([string]$file)

$lines = Get-Content $file

# --- Fix 5 (Pass 0a): Convert 'DEFB $XXYY' (4-digit hex = word) to 'DEFB $YY, $XX'
# Applied globally before any other pass so xinit collection (Pass 0b) sees correct sizes.
# The 4-digit form appears only in __xinit__ data for pointer/u16 fields.  Byte fields
# always use 2-digit hex ($XX), so this heuristic is safe.
$lines = $lines | ForEach-Object {
    $_ -replace '\bDEFB\s+\$([0-9a-fA-F]{2})([0-9a-fA-F]{2})\b', 'DEFB $$$2, $$$1'
}

# --- Fix 6 (Pass 0b): Collect __xinit__ data blocks ---
# Key: variable name (e.g. "cr" from "__xinit__cr").
# Value: list of DEFB lines (already word-expanded by Fix 5) to inline at the variable.
$xinitData = @{}
$inXinitBlock = $false
$currentXinitKey = $null
foreach ($line in $lines) {
    if ($line -match '^__xinit__(\w+):') {
        $currentXinitKey = $Matches[1]
        $xinitData[$currentXinitKey] = [System.Collections.Generic.List[string]]::new()
        $inXinitBlock = $true
    } elseif ($inXinitBlock -and $line -match '^\s+DEFB\s+') {
        $xinitData[$currentXinitKey].Add($line)
    } elseif ($inXinitBlock -and $line -notmatch '^\s') {
        # First non-indented line ends the current xinit block
        $inXinitBlock = $false
        $currentXinitKey = $null
        # Could be the start of another xinit block
        if ($line -match '^__xinit__(\w+):') {
            $currentXinitKey = $Matches[1]
            $xinitData[$currentXinitKey] = [System.Collections.Generic.List[string]]::new()
            $inXinitBlock = $true
        }
    }
}

# --- Pass 1: collect declared XREF symbols, defined labels, XDEF symbols ---
$xrefSyms = @{}
$xdefSyms = @{}
$definedLabels = @{}

foreach ($line in $lines) {
    if ($line -match '^\s+XREF\s+(\S+)')  { $xrefSyms[$Matches[1]]    = $true }
    if ($line -match '^\s+XDEF\s+(\S+)')  { $xdefSyms[$Matches[1]]    = $true }
    if ($line -match '^(\w+):')            { $definedLabels[$Matches[1]] = $true }
    # dot-prefixed local labels (e.g. "._random" or ".l_foo00101") — no colon, no XDEF
    if ($line -match '^\.(\w+)\s*$')       { $definedLabels[$Matches[1]] = $true }
}

# --- Pass 2: find call/jp targets that are neither XREF'd nor defined locally ---
$missingXrefs = [System.Collections.Generic.HashSet[string]]::new()
foreach ($line in $lines) {
    if ($line -match '^\s+(?:call|jp)\s+(_\w+)') {
        $sym = $Matches[1]
        if (-not $xrefSyms.ContainsKey($sym) -and -not $definedLabels.ContainsKey($sym)) {
            $missingXrefs.Add($sym) | Out-Null
        }
    }
}

# --- Pass 3: build output, inserting missing XREFs after the last XREF line ---
$lastXrefIdx = -1
for ($i = 0; $i -lt $lines.Count; $i++) {
    if ($lines[$i] -match '^\s+XREF\s+') { $lastXrefIdx = $i }
}

$result = [System.Collections.Generic.List[string]]::new()
$inInitArea      = $false
$pendingVarLabel = $null
$suppressXinit   = $false

for ($i = 0; $i -lt $lines.Count; $i++) {
    $line = $lines[$i]

    # --- Fix 6 output: track when we are inside the INITIALIZED area ---
    if ($line -match '^\s*;\s*Area\s+INITIALIZED\b') {
        $inInitArea      = $true
        $pendingVarLabel = $null
    } elseif ($line -match '^\s*;\s*Area\s+\S') {
        $inInitArea      = $false
        $pendingVarLabel = $null
    }

    # --- Fix 6 output: suppress __xinit__ label lines and their DEFB data ---
    if ($line -match '^__xinit__\w+:') {
        $suppressXinit = $true
        continue
    }
    if ($suppressXinit) {
        if ($line -match '^\s+DEFB\s+') { continue }
        $suppressXinit = $false
        # fall through: the line that ended the xinit block is still emitted below
    }

    # --- Fix 6 output: in INITIALIZED area, remember label then replace DEFS ---
    if ($inInitArea) {
        if ($line -match '^\s*\._([a-zA-Z_]\w*)\s*$') {
            $pendingVarLabel = $Matches[1]
        } elseif ($pendingVarLabel -ne $null -and $line -match '^\s+DEFS\s+') {
            if ($xinitData.ContainsKey($pendingVarLabel)) {
                # Emit the label line that preceded this DEFS (it is still in $result already)
                foreach ($dataLine in $xinitData[$pendingVarLabel]) {
                    $result.Add($dataLine)
                }
                $pendingVarLabel = $null
                continue   # skip the DEFS line
            }
            $pendingVarLabel = $null
        } elseif ($line -match '^\s') {
            # Any other indented line resets the pending label
            $pendingVarLabel = $null
        }
    }

    # Drop XDEF for symbols also declared XREF (they are external, not local)
    if ($line -match '^\s+XDEF\s+(\S+)' -and $xrefSyms.ContainsKey($Matches[1])) {
        continue
    }

    # Apply expression fixes
    $line = $line -replace '#([0-9a-fA-Fx])', '$1'   # strip '#' immediate prefix
    $line = $line -replace '~ \$FF\b', '& 0xff'       # old low-byte operator

    $result.Add($line)

    # After MODULE declaration, inject SECTION _CODE so code lands at 0x6A00
    if ($line -match '^\s+MODULE\s+') {
        $result.Add("`tSECTION _CODE")
    }

    # After the last XREF line, append missing XREF declarations
    if ($i -eq $lastXrefIdx -and $missingXrefs.Count -gt 0) {
        foreach ($sym in $missingXrefs) {
            $result.Add("`tXREF $sym")
        }
    }
}

# --- Fix 7: Patch SDCC void-call double-ptr dereference bug ---
# After a void __naked __sdcccall(1) call, SDCC incorrectly assumes A is preserved
# and emits 'ld (hl), a' (store) instead of 'ld a, (hl)' (load) as the first step
# of a double-pointer dereference.
# The store overwrites the lo-byte of the pointer with garbage from the void call,
# corrupting the pointer and causing a crash on the subsequent deref.
# Replace ld (hl),a with ld a,(hl) — turning the incorrect store into the correct load.
for ($i = 0; $i -lt $result.Count - 3; $i++) {
    if ($result[$i]   -match '^\s+ld\s+\(hl\),\s*a\s*$'  -and
        $result[$i+1] -match '^\s+inc\s+hl\s*$'           -and
        $result[$i+2] -match '^\s+ld\s+h,\s*\(hl\)\s*$'  -and
        $result[$i+3] -match '^\s+ld\s+l,\s*a\s*$') {
        $result[$i] = $result[$i] -replace 'ld\s+\(hl\),\s*a', 'ld  a, (hl)'
    }
}

$result | Set-Content $file
