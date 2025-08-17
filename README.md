/* Fecha: 17/08/2025 - 02:52  */
/* Fichero: .\README.md */
/* Último cambio: Añadida nota aclaratoria en la sección de compilación para reflejar la robustez tras la refactorización de la UI. */
/* Descripción: Documentación principal del proyecto, guía de inicio y visión general. */

# DIYTogether - Tu Mascota Virtual DIY

![Ilustración de DIYTogether en acción]([Link a git3.png])

**DIYTogether** no es solo un gadget. Es una aventura de aprendizaje, una puerta a la creatividad y una excusa para construir algo increíble con las personas que te importan.

## La Historia detrás de DIYTogether

Constantemente sueño despierto.

Fantaseo con ser *"ese tío guay"* para mis sobrinos, el que les enseña a crear, a entender la tecnología, a jugar con la electrónica y la domótica. Quería construir algo que nos uniera, un juego que les apasionara y despertara su curiosidad por dentro. Una excusa perfecta para aprender divirtiéndose y pasar tiempo de calidad juntos.

De esa idea nace **DIYTogether**.

Un proyecto que empezó con mis mejores amigos y que ahora abro al mundo, invitando a todo aquel que quiera unirse a crear una tecnología con alma.

---

## ¿Qué es DIYTogether?

DIYTogether es un proyecto de mascota virtual open-source basada en hardware ESP32. Es un **"Tamagotchi para Hackers"** diseñado desde cero para ser:

*   �� **Personalizable**: Crea y carga tus propios "Skins" (gráficos, animaciones y sonidos) sin necesidad de reprogramar nada. ¡Haz tu DIYTogether verdaderamente tuyo!
*   🧠 **Educativo**: Aprende electrónica, programación (C++/ESP-IDF) y conectividad IoT de una forma práctica y progresiva. Cada paso es una lección, y los errores son parte del viaje.
*   🌍 **Interactivo**: Utiliza sensores como el acelerómetro y la conectividad Bluetooth para interactuar con tu criatura de formas nuevas y creativas.
*   ❤️ **Abierto y Comunitario**: Todo el proyecto (firmware, hardware, diseños 3D) es libre. Queremos que lo copies, lo modifiques, lo mejores y lo compartas.

![Variedad de colores y criaturas DIYTogether]([Link a git2.png])

---

## Características Principales

*   **Hardware**: Basado en la placa Waveshare ESP32-C6-Touch-LCD-1.9 o hardware similar.
*   **Firmware**: Escrito en C/C++ con el framework ESP-IDF.
*   **Sistema de Evolución Modular**: Las criaturas evolucionan según tus cuidados, el entorno e interacciones.
*   **Licencias**:
    *   **Código**: GPLv3 - ¡Libre para siempre!
    *   **Diseños de la carcasa**: CC BY-NC - ¡Compártela, pero no para uso comercial sin permiso!

![Prototipo de DIYTogether con interfaz holográfica]([Link a git1.png])

---

### Estado Actual del Proyecto
> *Nota: Esta sección se actualizará con los avances.*

*   ✅ Prototipo funcional con interacciones básicas (alimentar, entrenar, limpiar).
*   ✅ Sistema de skins dinámicas implementado.
*   🚧 Trabajando en la conectividad Bluetooth con la App de soporte (FIYNDEX).
*   💡 Planeando una campaña de Kickstarter para ofrecer kits DIY asequibles.

---

## ¿Cómo puedes empezar?

### 1. Requisitos
*   **Hardware**: Waveshare ESP32-C6-Touch-LCD-1.9.
*   **Software**: [Visual Studio Code](https://code.visualstudio.com/) con la extensión [PlatformIO IDE](https://platformio.org/platformio-ide).

### 2. Compilación y Subida del Firmware
Este proyecto está configurado para ser compilado y subido fácilmente con PlatformIO.

*(Nota: La estructura de componentes ha sido refactorizada para mejorar la modularidad, pero el proceso de compilación con PlatformIO no ha cambiado y sigue siendo el método recomendado.)*

1.  **Clona el repositorio**:
    ` ash
    git clone [URL de tu repositorio]
    cd DIYtogether
    `
2.  **Abre el proyecto en VS Code**:
    *   Abre VS Code.
    *   Ve a File > Open Folder... y selecciona la carpeta DIYtogether que acabas de clonar.
    *   PlatformIO debería detectar automáticamente el fichero platformio.ini y configurar el entorno.

3.  **Compila y Sube**:
    *   Conecta tu placa ESP32-C6 al ordenador por USB.
    *   En la barra de estado de PlatformIO (generalmente en la parte inferior de VS Code), busca y haz clic en el icono de la flecha (->) que corresponde a **Upload**.
    *   PlatformIO compilará el código, generará los binarios y los subirá automáticamente a tu placa.

4.  **Monitor Serie**:
    *   Para ver los logs de depuración, haz clic en el icono del enchufe (🔌) que corresponde a **Monitor** en la barra de estado de PlatformIO.

*(Aquí puedes poner un GIF animado de tu prototipo funcionando. ¡Esto es súper efectivo!)*
![alt text](ruta/a/tu/gif/animado.gif)

---

## ¡Únete a la Comunidad!

Este proyecto vive gracias a gente como tú. Si te gusta la idea, ¡involúcrate!

*   ⭐ **Dale una estrella a este repositorio**: ¡Nos ayuda a ganar visibilidad!
*   💬 **Únete a nuestro canal de Discord/Telegram**: El mejor lugar para charlar, pedir ayuda y compartir tus creaciones.
*   💡 **Aporta ideas o reporta bugs**: Abre un "Issue" aquí en GitHub.
*   🚀 **Contribuye con código o diseños**: ¡Los Pull Requests son bienvenidos! Revisa nuestra [guía de contribución].

Gracias por tu apoyo y por creer en un proyecto hecho con cariño y curiosidad.
