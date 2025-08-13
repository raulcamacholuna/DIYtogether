/* Fichero: script.js */
document.addEventListener('DOMContentLoaded', () => {
    const uploadForm = document.getElementById('upload-form');
    const fileInput = document.getElementById('file-input');
    const statusDiv = document.getElementById('upload-status');

    uploadForm.addEventListener('submit', async (event) => {
        event.preventDefault();

        if (!fileInput.files.length) {
            showStatus('Por favor, selecciona un archivo.', 'error');
            return;
        }

        const file = fileInput.files[0];
        showStatus(`Subiendo ${file.name}...`, 'info');

        try {
            // Anotación: No se usa FormData para un flujo de subida más simple
            // que el manejador en C pueda procesar.
            const response = await fetch('/upload', {
                method: 'POST',
                headers: {
                    'Content-Disposition': `attachment; filename="${file.name}"`
                },
                body: file
            });

            const resultText = await response.text();

            if (response.ok) {
                showStatus(resultText, 'success');
            } else {
                showStatus(`Error: ${resultText}`, 'error');
            }

        } catch (error) {
            console.error('Error de red:', error);
            showStatus('Error de conexión con el dispositivo.', 'error');
        } finally {
            uploadForm.reset();
        }
    });

    function showStatus(message, type) {
        statusDiv.style.display = 'block';
        statusDiv.textContent = message;
        statusDiv.className = type; // 'success' o 'error'
    }
});