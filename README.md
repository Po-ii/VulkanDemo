# VulkanDemo

Démo minimale de rendu 3D avec **Vulkan**, écrite en **C++23** pour Windows. Le projet initialise une fenêtre GLFW, crée les objets Vulkan nécessaires au rendu, charge une texture et utilise un shader Slang compilé en SPIR-V pour faire apparaître un cube texturé.

## Fonctionnalités

- Création d’une fenêtre Vulkan redimensionnable avec GLFW.
- Initialisation de l’instance Vulkan.
- Sélection du périphérique physique.
- Création de la surface Vulkan.
- Gestion de la swap chain.
- Création d’un pipeline graphique.
- Utilisation de vertex buffers et uniform buffers.
- Gestion d’un depth buffer.
- Chargement d’une texture `.jpg` avec `stb_image`.
- Compilation d’un shader Slang vers SPIR-V.
- Synchronisation des images.
- Gestion du redimensionnement de la fenêtre.
- Configurations Visual Studio `Debug` et `Release`.
- Support des plateformes `Win32` et `x64`.

## Prérequis

Le projet est actuellement configuré pour Windows et Visual Studio.

- Windows avec une carte graphique compatible Vulkan.
- Pilotes graphiques Vulkan à jour.
- Visual Studio.
- Vulkan SDK `1.4.357.0`.
- Compilateur Slang fourni avec le Vulkan SDK.
- Git, si le projet est cloné depuis GitHub.

Le dépôt contient déjà plusieurs dépendances :

- GLFW dans `include/GLFW` et `lib/GLFW`.
- `stb_image.h` dans `include/`.
- Le shader source dans `shaders/shader.slang`.
- Le shader compilé dans `shaders/slang.spv`.
- La texture utilisée par l’application dans `textures/texture.jpg`.

## Télécharger la release

La release **VulkanDemo 1.0.0** est disponible au téléchargement :

- [Télécharger VulkanDemo-1.0.0.zip](https://github.com/Po-ii/VulkanDemo/releases/tag/gold)
  
## Installation

Cloner le dépôt avec Git :

```powershell
git clone https://github.com/Po-ii/VulkanDemo.git
cd VulkanDemo
```

Vérifier que la structure suivante est disponible :

```text
.
├── VulkanDemo.slnx
├── VulkanDemo/
├── include/
├── lib/
├── shaders/
└── textures/
```

Installer ensuite :

2. Le Vulkan SDK.

## Versions et configuration

| Élément | Version ou configuration |
| --- | --- |
| Langage | C++23 |
| Vulkan SDK | 1.4.357.0 |
| Toolset Visual C++ | `v145` |
| Plateformes | Win32 et x64 |
| Configurations | Debug et Release |
| Fenêtre | GLFW |
| API graphique GLFW | Désactivée pour utiliser Vulkan |

Les chemins du Vulkan SDK sont actuellement configurés directement dans le projet Visual Studio :

```text
C:\VulkanSDK\1.4.357.0\Include
C:\VulkanSDK\1.4.357.0\Lib
```

Si le Vulkan SDK est installé dans un autre dossier, modifier les chemins dans :

```text
VulkanDemo/VulkanDemo.vcxproj
shaders/compile.bat
```

## Compilation C++

### Avec Visual Studio

1. Ouvrir `VulkanDemo.slnx`.
2. Sélectionner une configuration :
   - `Debug` ou `Release`.
   - `x64` de préférence.
3. Vérifier les chemins du Vulkan SDK.
4. Compiler avec :

```text
Build > Build Solution
```

Les configurations disponibles sont :

```text
Debug|Win32
Release|Win32
Debug|x64
Release|x64
```

L’exécutable est généralement généré dans un dossier similaire à :

```text
VulkanDemo/x64/Debug/VulkanDemo.exe
```

ou :

```text
VulkanDemo/x64/Release/VulkanDemo.exe
```

Le chemin exact dépend de la configuration utilisée par Visual Studio.

## Exécution

Avant de lancer l’application :

1. Compiler le shader avec `shaders/compile.bat`.
2. Compiler le projet C++.
3. Vérifier que `textures/texture.jpg` existe.
4. Lancer l’exécutable depuis un emplacement compatible avec les chemins relatifs utilisés par le projet.

Exemple :

```powershell
cd VulkanDemo/x64/Debug
./VulkanDemo.exe
```

L’application crée une fenêtre nommée `VulkanDemo`.

La boucle principale :

- traite les événements GLFW ;
- dessine une frame ;
- attend que le GPU termine son travail avant de quitter.

Pour fermer l’application, fermer la fenêtre.

## Architecture du projet

```text
.
├── VulkanDemo.slnx
│
├── VulkanDemo/
│   ├── Application.cpp
│   ├── CommandManager.cpp
│   ├── Debug.cpp
│   ├── DepthBuffer.cpp
│   ├── DescriptorManager.cpp
│   ├── Device.cpp
│   ├── FrameSynchronizer.cpp
│   ├── GraphicPipeline.cpp
│   ├── Instance.cpp
│   ├── Renderer.cpp
│   ├── Surface.cpp
│   ├── SwapChain.cpp
│   ├── Texture.cpp
│   ├── UniformBuffer.cpp
│   ├── VertexBuffer.cpp
│   ├── VulkanDemo.cpp
│   └── VulkanDemo.vcxproj
│
├── include/
│   ├── GLFW/
│   └── stb_image.h
│
├── lib/
│   └── GLFW/
│
├── shaders/
│   ├── shader.slang
│   ├── slang.spv
│   └── compile.bat
│
└── textures/
    └── texture.jpg
```

### Principaux composants

| Fichier ou composant | Rôle |
| --- | --- |
| `Application` | Initialise la fenêtre et l’application |
| `Instance` | Crée l’instance Vulkan |
| `Device` | Gère le périphérique physique et logique |
| `Surface` | Gère la surface de présentation |
| `SwapChain` | Gère les images présentées à l’écran |
| `GraphicPipeline` | Configure le pipeline graphique |
| `VertexBuffer` | Gère les sommets |
| `UniformBuffer` | Gère les matrices et données uniformes |
| `Texture` | Charge et configure la texture |
| `DepthBuffer` | Gère le tampon de profondeur |
| `DescriptorManager` | Gère les descripteurs Vulkan |
| `FrameSynchronizer` | Gère les sémaphores et fences |
| `Renderer` | Coordonne le rendu |

## Limites connues

- Le projet est actuellement orienté Windows et Visual Studio.
- Les chemins du Vulkan SDK sont codés en dur.
- Le script `compile.bat` dépend d’une installation précise du Vulkan SDK.
- Le chargement de texture utilise un chemin relatif fixe.
- Une seule texture est utilisée par la démo.
- Un seul fichier SPIR-V est utilisé.
- Le shader utilise la texture, mais la couleur des sommets n’est pas utilisée dans le résultat final.
- La gestion des erreurs peut encore être améliorée.
- La compatibilité dépend des fonctionnalités Vulkan supportées par le GPU et ses pilotes.
- Les versions Win32 et x64 sont présentes dans le projet, mais x64 est recommandée.

## Références

- [Dépôt VulkanDemo](https://github.com/Po-ii/VulkanDemo)
- [Vulkan SDK — LunarG](https://vulkan.lunarg.com/)
- [Vulkan Tutorial](https://docs.vulkan.org/tutorial/latest/00_Introduction.html)
- [stb_image](https://github.com/nothings/stb)
