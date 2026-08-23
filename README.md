# Vulkan Final Project

## Descripcion

Escena 3D estilo Mario hecha con Vulkan.
La escena representa un pequeño nivel: Mario salta debajo de un bloque flotante,
la estrella sale del bloque, hay una tuberia a un lado y Lakitu sale flotando desde esa zona.
Tambien hay una esfera skybox de cielo estilo Mario centrada en la camara.
El proyecto carga modelos OBJ, aplica matrices MVP independientes por objeto,
usa texturas, iluminacion Phong y una camara FPS con perspectiva.

## Requisitos de la rubrica cubiertos

- **Carga de modelos 3D / animacion:** mas de 5 instancias de objetos en escena:
  - suelo de ladrillo
  - esfera de cielo
  - Mario
  - Goomba
  - tuberia
  - bloque
  - estrella
  - Lakitu
  - particulas de brillo alrededor de la estrella
- **Tipos distintos de OBJ:** mas de 3 formas OBJ distintas:
  - `plane.obj`
  - `mario.obj`
  - `goomba.obj`
  - `warp_pipe.obj`
  - `marioblock.obj`
  - `star.obj`
  - `lakitu.obj`
- **Texturas:** mas de 5 texturas distintas:
  - `bricks.bmp`
  - `mariosky.bmp`
  - `mario.bmp`
  - `goomba.png`
  - `warp_pipe.png`
  - `marioblock.png`
  - `rainbowStarMario.bmp`
  - `lakitu.bmp`
- **Matrices de modelo:** cada objeto tiene su propia matriz `model`, con posicion, rotacion y escala.
- **Animacion:** varios objetos cambian su matriz de modelo cada frame:
  - Mario salta verticalmente debajo del bloque
  - Goomba se mueve de lado a lado
  - el bloque flotante se mantiene estatico
  - la estrella sale hacia arriba desde el bloque
  - Lakitu sube/flota desde la zona de la tuberia
  - varias particulas se mueven alrededor de la estrella
- **Camara:** camara FPS con teclado, raton y proyeccion en perspectiva.
- **Iluminacion:** modelo Phong en el fragment shader.
- **Funcion avanzada:** sky sphere y animacion de particulas. La esfera de cielo sigue la posicion de la camara, y las particulas son pequenas estrellas animadas alrededor de la estrella principal.
- **Estructuras y calidad:** se usa la estructura `SceneObject`, vectores para los objetos y funciones separadas para cargar recursos, actualizar matrices, actualizar luces, dibujar y limpiar memoria.

## Funcion avanzada

Las funciones avanzadas elegidas son **sky sphere** y **animacion de particulas**.
La sky sphere usa `sphere.obj` con la textura `mariosky.bmp`.
Cada frame se coloca en la posicion de la camara para que el jugador siempre este dentro del cielo.
Se desactiva el culling de caras para poder ver la esfera desde dentro.

La escena tiene varias particulas de brillo alrededor de la estrella principal.
Cada particula usa el modelo `star.obj` con una escala muy pequena y una matriz de modelo propia.

Las particulas se animan cada frame con funciones `sin` y `cos`:

- se desplazan suavemente en X e Y
- suben y bajan en Z
- giran alrededor de su centro

Esta opcion esta en la lista de funciones avanzadas del enunciado y mantiene el codigo sencillo.

## Controles

- `W`: avanzar
- `S`: retroceder
- `A`: moverse a la izquierda
- `D`: moverse a la derecha
- `Q`: bajar
- `E`: subir
- Raton: orientar la camara
- `Esc`: cerrar la aplicacion

## Como compilar

1. Abrir `VulkanTest.sln` con Visual Studio 2022.
2. Seleccionar configuracion `Debug` y plataforma `x64`.
3. Compilar el proyecto `VulkanTest`.
4. Ejecutar desde Visual Studio.

Los shaders ya estan compilados como `vert.spv` y `frag.spv`. Si se modifican
`shader.vert` o `shader.frag`, ejecutar `VulkanTest/VulkanTest/compile_shaders.bat`
desde la carpeta `VulkanTest/VulkanTest`.

## Archivos principales

- `VulkanTest/VulkanTest/main.cpp`: escena de Mario, camara, animacion, descriptores y render loop.
- `VulkanTest/VulkanTest/vulkan_setup.cpp`: inicializacion de Vulkan, swapchain, pipeline y depth buffer.
- `VulkanTest/VulkanTest/model.cpp`: carga de modelos OBJ con tinyobjloader.
- `VulkanTest/VulkanTest/texture.cpp`: carga de texturas con stb_image.
- `VulkanTest/VulkanTest/shader.vert`: transformacion MVP y normales.
- `VulkanTest/VulkanTest/shader.frag`: textura + iluminacion Phong.

## Referencias y fuentes

- Plantilla Vulkan proporcionada en eStudy.
- Apuntes de la asignatura: Camera, Load OBJ, Texture and Light.
- Modelos y texturas de Mario proporcionados en los ZIPs de la practica.
- tinyobjloader para cargar OBJ.
- stb_image para cargar imagenes.
- LearnOpenGL Camera: https://learnopengl.com/Getting-started/Camera
