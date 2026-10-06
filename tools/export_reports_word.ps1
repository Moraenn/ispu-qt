param([string]$DocumentDirectory, [string]$OutputDirectory)
$ErrorActionPreference = 'Stop'
$documents = (Resolve-Path -LiteralPath $DocumentDirectory).Path
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null
$output = (Resolve-Path -LiteralPath $OutputDirectory).Path
$word = New-Object -ComObject Word.Application
$word.Visible = $false
$word.DisplayAlerts = 0
try {
    foreach ($file in Get-ChildItem -LiteralPath $documents -Filter '*.docx') {
        $document = $word.Documents.Open($file.FullName, $false, $true)
        try {
            $document.Repaginate()
            $pages = $document.ComputeStatistics(2)
            $document.ExportAsFixedFormat((Join-Path $output ($file.BaseName + '.pdf')), 17)
            Write-Output ($file.Name + ': ' + $pages + ' pages')
        } finally { $document.Close(0) }
    }
} finally { $word.Quit() }
