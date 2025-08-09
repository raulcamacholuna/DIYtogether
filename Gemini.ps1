# Detiene el script si ocurre algun error
$ErrorActionPreference = "Stop"

# --- Configuracion ---
$outFile = "gemini.txt"
$maxSizeBytes = 102400 # 100 KB
$includePatterns = @("*.c", "*.h", "CMakeLists.txt", "*.yml")
$excludeDirs = @("build", ".vscode")

# --- Encabezado y Limpieza ---
Clear-Host
Write-Host "=============================================================" -ForegroundColor Green
Write-Host "      Recopilador de Codigo PowerShell para Gemini" -ForegroundColor Green
Write-Host "============================================================="
Write-Host
Write-Host "Buscando ficheros: $($includePatterns -join ', ')"
Write-Host "[EXCLUYENDO]: Directorios '$($excludeDirs -join "', '")'" -ForegroundColor Yellow
Write-Host "[FILTRANDO] : Ficheros > 100 KB" -ForegroundColor Yellow
Write-Host "[GUARDANDO EN]: $outFile"
Write-Host
Write-Host "-------------------------------------------------------------"
Write-Host

if (Test-Path $outFile) {
    Remove-Item $outFile
}

# --- Logica Principal (Robusta y Eficiente) ---
# 1. Obtiene recursivamente todos los items.
# 2. Excluye los directorios especificados.
# 3. Incluye solo los patrones de fichero deseados.
# 4. Procesa cada fichero encontrado.
Get-ChildItem -Path . -Recurse -Exclude $excludeDirs | Where-Object { $_.Name -in $includePatterns -or $includePatterns -contains $_.Extension } | ForEach-Object {
    $file = $_
    
    if ($file.Length -lt $maxSizeBytes) {
        Write-Host ("INCLUIDO  : " + $file.FullName + " (" + $file.Length + " bytes)") -ForegroundColor Cyan
        
        # Anade la cabecera y el contenido al fichero de salida
        Add-Content -Path $outFile -Value "--- START OF FILE $($file.FullName) ---"
        Add-Content -Path $outFile -Value (Get-Content -Path $file.FullName -Raw)
        Add-Content -Path $outFile -Value ""
    } else {
        Write-Host ("OMITIDO   : " + $file.FullName + " (" + $file.Length + " bytes)") -ForegroundColor Gray
    }
}

# --- Pie de Pagina ---
Write-Host
Write-Host "-------------------------------------------------------------"
Write-Host
Write-Host "=============================================================" -ForegroundColor Green
Write-Host "      Proceso completado." -ForegroundColor Green
Write-Host "============================================================="
Write-Host

Read-Host "Presiona Enter para salir"