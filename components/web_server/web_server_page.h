/*
# Fichero: Z:\DIYTOGETHER\DIYtogether\components\web_server\web_server_page.h
# Fecha: `$timestamp
# Último cambio: Implementada navegación por directorios y subida de archivos contextual.
# Descripción: Contiene la página HTML principal del portal de configuración. Se ha mejorado la interfaz para permitir la navegación por subdirectorios en la tarjeta SD y subir archivos al directorio actual.
*/
#ifndef WEB_SERVER_PAGE_H
#define WEB_SERVER_PAGE_H

static const char* INDEX_HTML_CONTENT = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Gestor de Archivos ESP32</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); min-height: 100vh; color: #333; }
        .container { max-width: 800px; margin: 0 auto; padding: 20px; }
        .header { text-align: center; margin-bottom: 30px; color: white; }
        .header h1 { font-size: 2.5em; margin-bottom: 10px; text-shadow: 2px 2px 4px rgba(0,0,0,0.3); }
        .header p { font-size: 1.2em; opacity: 0.9; }
        .card { background: white; border-radius: 15px; padding: 25px; margin-bottom: 20px; box-shadow: 0 5px 15px rgba(0,0,0,0.1); }
        .card h3 { color: #5a67d8; margin-bottom: 20px; font-size: 1.4em; word-break: break-all; }
        .btn { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; border: none; padding: 12px 25px; border-radius: 25px; font-size: 16px; font-weight: 600; cursor: pointer; transition: all 0.3s ease; box-shadow: 0 4px 15px rgba(0,0,0,0.2); }
        .btn:hover { transform: translateY(-2px); box-shadow: 0 6px 20px rgba(0,0,0,0.3); }
        .btn-secondary { background: linear-gradient(135deg, #4fd1c7 0%, #06b6d4 100%); }
        .btn-danger { background: linear-gradient(135deg, #fc8181 0%, #f56565 100%); }
        .file-list { max-height: 400px; overflow-y: auto; border: 1px solid #e2e8f0; border-radius: 10px; margin-top: 10px; }
        .file-item { display: flex; justify-content: space-between; align-items: center; padding: 12px 15px; border-bottom: 1px solid #f0f2f5; transition: background 0.3s ease; }
        .file-item:hover { background: #f7fafc; }
        .file-item:last-child { border-bottom: none; }
        .file-info { display: flex; align-items: center; gap: 10px; word-break: break-all; }
        .file-info a { color: #4a5568; text-decoration: none; font-weight: 600; }
        .file-info a:hover { color: #5a67d8; }
        .file-actions { display: flex; gap: 10px; }
        .btn-small { padding: 6px 12px; font-size: 12px; border-radius: 15px; }
        .progress-bar { width: 100%; height: 8px; background: #e2e8f0; border-radius: 4px; overflow: hidden; margin: 10px 0; }
        .progress-fill { height: 100%; background: linear-gradient(90deg, #48bb78, #38a169); border-radius: 4px; transition: width 0.3s ease; }
        .upload-area { border: 3px dashed #cbd5e0; border-radius: 15px; padding: 40px 20px; text-align: center; cursor: pointer; transition: all 0.3s ease; background: rgba(90, 103, 216, 0.02); }
        .upload-area:hover, .upload-area.dragover { border-color: #5a67d8; background: rgba(90, 103, 216, 0.05); }
        .hidden { display: none !important; }
        .status { font-size: 0.9em; color: #718096; margin-top: 10px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>📦 Gestor de Archivos</h1>
            <p>Sube y gestiona los archivos de tu dispositivo</p>
        </div>

        <div class="card">
            <h3>📤 Subir a: <span id="upload-path">/</span></h3>
            <div id="upload-area" class="upload-area">
                <h4>Arrastra archivos aquí o haz clic para seleccionar</h4>
            </div>
            <input type="file" id="file-input" multiple class="hidden">
            
            <div style="margin-top: 20px;">
                <label>Progreso de Subida</label>
                <div class="progress-bar">
                    <div class="progress-fill" id="upload-progress" style="width: 0%"></div>
                </div>
                <span id="upload-status" class="status">Listo para subir archivos.</span>
            </div>
        </div>

        <div class="card">
            <h3>📁 Archivos en: <span id="current-path">/</span></h3>
            <div style="margin-bottom: 15px;">
                <button class="btn btn-secondary" id="refresh-btn">🔄 Actualizar</button>
            </div>
            <div class="file-list" id="file-list"></div>
        </div>
    </div>

    <script>
        document.addEventListener('DOMContentLoaded', () => {
            let currentPath = '/';
            const uploadArea = document.getElementById('upload-area');
            const fileInput = document.getElementById('file-input');
            const fileListDiv = document.getElementById('file-list');
            const refreshBtn = document.getElementById('refresh-btn');
            const currentPathSpan = document.getElementById('current-path');
            const uploadPathSpan = document.getElementById('upload-path');

            function formatBytes(bytes, decimals = 2) {
                if (!+bytes) return '0 Bytes'
                const k = 1024
                const dm = decimals < 0 ? 0 : decimals
                const sizes = ['Bytes', 'KB', 'MB', 'GB', 'TB']
                const i = Math.floor(Math.log(bytes) / Math.log(k))
                return `${parseFloat((bytes / Math.pow(k, i)).toFixed(dm))} ${sizes[i]}`
            }

            async function fetchFileList(path) {
                currentPath = path;
                currentPathSpan.textContent = path;
                uploadPathSpan.textContent = path;
                fileListDiv.innerHTML = '<p>Cargando lista de archivos...</p>';
                try {
                    const response = await fetch(`/listfiles?path=${encodeURIComponent(path)}`);
                    if (!response.ok) throw new Error('Error del servidor: ' + response.statusText);
                    const files = await response.json();
                    renderFileList(files, path);
                } catch (error) {
                    console.error('Error al obtener la lista de archivos:', error);
                    fileListDiv.innerHTML = '<p style="color: red;">No se pudo cargar la lista. Revisa la conexión.</p>';
                }
            }

            async function handleFileUpload(files) {
                const progressBar = document.getElementById('upload-progress');
                const statusText = document.getElementById('upload-status');
                if (files.length === 0) return;

                statusText.textContent = `Iniciando subida de ${files.length} archivo(s) a ${currentPath}...`;
                progressBar.style.width = '0%';
                let filesUploaded = 0;

                for (const file of files) {
                    const formData = new FormData();
                    formData.append('path', currentPath);
                    formData.append('file', file, file.name);

                    try {
                        statusText.textContent = `Subiendo ${file.name}...`;
                        const response = await fetch('/upload', { method: 'POST', body: formData });
                        if (!response.ok) throw new Error(`Fallo en la subida del archivo ${file.name}`);
                        filesUploaded++;
                        progressBar.style.width = `${(filesUploaded / files.length) * 100}%`;
                    } catch (error) {
                        console.error(error);
                        statusText.innerHTML = `<span style="color: red;">❌ Error al subir ${file.name}.</span>`;
                        await new Promise(resolve => setTimeout(resolve, 3000));
                    }
                }
                
                statusText.innerHTML = '<span style="color: green;">✅ Subida completada.</span>';
                fetchFileList(currentPath);
                setTimeout(() => {
                    progressBar.style.width = '0%';
                    statusText.textContent = 'Listo para subir archivos.';
                }, 2000);
            }
            
            async function deleteFile(path, filename) {
                if (!confirm(`⚠️ ¿Seguro que quieres borrar "${filename}"?`)) return;
                
                try {
                    const formData = new FormData();
                    formData.append('path', path);
                    formData.append('filename', filename);
                    
                    const response = await fetch('/delete', { method: 'POST', body: formData });
                    if (!response.ok) throw new Error('El servidor no pudo borrar el archivo.');
                    
                    console.log(`Archivo "${filename}" borrado.`);
                    fetchFileList(currentPath); 
                } catch (error) {
                    console.error('Error al borrar:', error);
                    alert(`No se pudo borrar: ${filename}`);
                }
            }

            function renderFileList(files, path) {
                let html = '';
                if (path !== '/') {
                    const parentPath = path.substring(0, path.lastIndexOf('/')) || '/';
                    html += `
                        <div class="file-item">
                          <div class="file-info">
                            <span style="font-size: 1.2em;">⤴️</span>
                            <a href="#" class="dir-link" data-path="${parentPath}">.. (Subir un nivel)</a>
                          </div>
                        </div>`;
                }

                if (!files || files.length === 0) {
                    fileListDiv.innerHTML = html + '<p style="padding:15px;">Este directorio está vacío.</p>';
                    return;
                }
                
                files.sort((a, b) => {
                    if (a.type === b.type) return a.name.localeCompare(b.name);
                    return a.type === 'dir' ? -1 : 1;
                }).forEach(file => {
                    const icon = file.type === 'dir' ? '📁' : '📄';
                    const sizeFormatted = file.type === 'file' ? formatBytes(file.size) : '';
                    const fullPath = path === '/' ? `/${file.name}` : `${path}/${file.name}`;
                    const nameHtml = file.type === 'dir' 
                        ? `<a href="#" class="dir-link" data-path="${fullPath}">${file.name}</a>`
                        : `<span>${file.name} <small style="color:#888">(${sizeFormatted})</small></span>`;
                        
                    html += `
                        <div class="file-item">
                          <div class="file-info">
                            <span style="font-size: 1.2em;">${icon}</span>
                            ${nameHtml}
                          </div>
                          <div class="file-actions">
                            <button class="btn btn-small btn-danger" data-path="${path}" data-filename="${file.name}">🗑️ Borrar</button>
                          </div>
                        </div>`;
                });
                fileListDiv.innerHTML = html;
            }
            
            uploadArea.addEventListener('click', () => fileInput.click());
            fileInput.addEventListener('change', (e) => handleFileUpload(e.target.files));

            ['dragenter', 'dragover', 'dragleave', 'drop'].forEach(evt => uploadArea.addEventListener(evt, e => { e.preventDefault(); e.stopPropagation(); }));
            ['dragenter', 'dragover'].forEach(evt => uploadArea.addEventListener(evt, () => uploadArea.classList.add('dragover')));
            ['dragleave', 'drop'].forEach(evt => uploadArea.addEventListener(evt, () => uploadArea.classList.remove('dragover')));
            uploadArea.addEventListener('drop', e => handleFileUpload(e.dataTransfer.files));

            fileListDiv.addEventListener('click', e => {
                if (e.target && e.target.closest('a.dir-link')) {
                    e.preventDefault();
                    const path = e.target.closest('a.dir-link').getAttribute('data-path');
                    fetchFileList(path);
                }
                if (e.target && e.target.closest('button.btn-danger')) {
                    const btn = e.target.closest('button.btn-danger');
                    const path = btn.getAttribute('data-path');
                    const filename = btn.getAttribute('data-filename');
                    if (path && filename) deleteFile(path, filename);
                }
            });

            refreshBtn.addEventListener('click', () => fetchFileList(currentPath));
            
            fetchFileList(currentPath);
        });
    </script>
</body>
</html>
)rawliteral";

#endif // WEB_SERVER_PAGE_H
