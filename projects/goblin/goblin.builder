{
    "format": "8bit-Unity Project", 
    "formatVersion": 2, 
    "general": {
        "disk": "goblin", 
        "code": [
            "projects/goblin/src/goblin.c", 
            "projects/goblin/src/interface.c", 
            "projects/goblin/src/inventory.c", 
            "projects/goblin/src/scene.c"
        ], 
        "shared": [
            "projects/goblin/menu.txt", 
            "projects/goblin/navigation/ending.nav", 
            "projects/goblin/navigation/scene01.nav", 
            "projects/goblin/navigation/scene02.nav", 
            "projects/goblin/navigation/scene03.nav"
        ], 
        "charmap": [], 
        "tilesize": "None"
    }, 
    "platform": {
        "Apple": {
            "spriteFrames": "15", 
            "spriteWidth": "7", 
            "spriteHeight": "24", 
            "bitmapDHR": [
                "projects/goblin/bitmaps/menu-apple.png", 
                "projects/goblin/bitmaps/ending-apple.png", 
                "projects/goblin/bitmaps/scene01-apple.png", 
                "projects/goblin/bitmaps/scene02-apple.png", 
                "projects/goblin/bitmaps/scene03-apple.png"
            ], 
            "bitmapSHR": [], 
            "charsetDHR": [], 
            "charsetSHR": [], 
            "spritesDHR": [
                "projects/goblin/sprites/sprites-apple.png"
            ], 
            "spritesSHR": [], 
            "music": [
                "projects/goblin/music/menu-apple.m"
            ], 
            "chunks": [
                "projects/goblin/chunks/scene01-apple.txt", 
                "projects/goblin/chunks/scene02-apple.txt", 
                "projects/goblin/chunks/scene03-apple.txt"
            ], 
            "diskSize": "140KB", 
            "crunchAssets": "No", 
            "musicMemory": "auto", 
            "chunkMemory": "$2000", 
            "network8bitHub": false, 
            "networkUthernet": false, 
            "networkProtocols": "TCP/UDP"
        }, 
        "Atari": {
            "spriteFrames": "15", 
            "spriteWidth": "8", 
            "spriteHeight": "26", 
            "bitmap": [
                "projects/goblin/bitmaps/menu-atari.png", 
                "projects/goblin/bitmaps/ending-atari.png", 
                "projects/goblin/bitmaps/scene01-atari.png", 
                "projects/goblin/bitmaps/scene02-atari.png", 
                "projects/goblin/bitmaps/scene03-atari.png"
            ], 
            "charset": [], 
            "sprites": [
                "projects/goblin/sprites/sprites-atari.png"
            ], 
            "music": [
                "projects/goblin/music/menu-atari.rmt"
            ], 
            "chunks": [
                "projects/goblin/chunks/scene01-atari.txt", 
                "projects/goblin/chunks/scene02-atari.txt", 
                "projects/goblin/chunks/scene03-atari.txt"
            ], 
            "diskSize": "180KB", 
            "crunchAssets": "No", 
            "musicMemory": "$0800", 
            "chunkMemory": "$2000", 
            "network8bitHub": false, 
            "networkDragonCart": false, 
            "networkFujinet": false, 
            "networkProtocols": "TCP/UDP"
        }, 
        "C64": {
            "spriteFrames": "15", 
            "spriteWidth": "12", 
            "spriteHeight": "21", 
            "bitmap": [
                "projects/goblin/bitmaps/menu-c64.png", 
                "projects/goblin/bitmaps/ending-c64.png", 
                "projects/goblin/bitmaps/scene01-c64.png", 
                "projects/goblin/bitmaps/scene02-c64.png", 
                "projects/goblin/bitmaps/scene03-c64.png"
            ], 
            "charset": [], 
            "charsetColors": " ", 
            "sprites": [
                "projects/goblin/sprites/sprites-c64.png"
            ], 
            "music": [
                "projects/goblin/music/menu-c64.sid"
            ], 
            "chunks": [
                "projects/goblin/chunks/scene01-c64.txt", 
                "projects/goblin/chunks/scene02-c64.txt", 
                "projects/goblin/chunks/scene03-c64.txt"
            ], 
            "crunchAssets": "No", 
            "musicMemory": "$4000", 
            "chunkMemory": "$2000", 
            "network8bitHub": false, 
            "networkRRNet": false, 
            "networkUltimate": false, 
            "networkProtocols": "TCP/UDP"
        }, 
        "Lynx": {
            "spriteFrames": "15", 
            "spriteWidth": "9", 
            "spriteHeight": "13", 
            "bitmap": [
                "projects/goblin/bitmaps/menu-lynx.png", 
                "projects/goblin/bitmaps/ending-lynx.png", 
                "projects/goblin/bitmaps/scene01-lynx.png", 
                "projects/goblin/bitmaps/scene02-lynx.png", 
                "projects/goblin/bitmaps/scene03-lynx.png"
            ], 
            "charset": [], 
            "sprites": [
                "projects/goblin/sprites/sprites-lynx.png"
            ], 
            "music": [
                "projects/goblin/music/menu-lynx.asm"
            ], 
            "chunks": [
                "projects/goblin/chunks/scene01-lynx.txt", 
                "projects/goblin/chunks/scene02-lynx.txt", 
                "projects/goblin/chunks/scene03-lynx.txt"
            ], 
            "musicMemory": "$0800", 
            "chunkMemory": "$2000", 
            "sharedMemory": "$0600", 
            "virtualKeyboard": false
        }, 
        "NES": {
            "bitmapTiles": "192", 
            "spriteFrames": "15", 
            "spriteWidth": "16", 
            "spriteHeight": "16", 
            "bitmap": [
                "projects/goblin/bitmaps/menu-nes.png", 
                "projects/goblin/bitmaps/ending-nes.png", 
                "projects/goblin/bitmaps/scene01-nes.png", 
                "projects/goblin/bitmaps/scene02-nes.png", 
                "projects/goblin/bitmaps/scene03-nes.png"
            ], 
            "charset": [], 
            "sprites": [
                "projects/goblin/sprites/sprites-nes.png"
            ], 
            "music": [
                "projects/goblin/music/menu-nes.txt"
            ], 
            "chunks": [
                "projects/goblin/chunks/scene01-nes.txt", 
                "projects/goblin/chunks/scene02-nes.txt"
            ], 
            "musicMemory": "$4000", 
            "chunkMemory": "$0400"
        }, 
        "Oric": {
            "spriteFrames": "15", 
            "spriteWidth": "12", 
            "spriteHeight": "24", 
            "dithering": "0.2", 
            "leadColors": " ", 
            "bitmap": [
                "projects/goblin/bitmaps/menu-oric.png", 
                "projects/goblin/bitmaps/ending-oric.png", 
                "projects/goblin/bitmaps/scene01-oric.png", 
                "projects/goblin/bitmaps/scene02-oric.png", 
                "projects/goblin/bitmaps/scene03-oric.png"
            ], 
            "charset": [], 
            "sprites": [
                "projects/goblin/sprites/sprites-oric.png"
            ], 
            "music": [
                "projects/goblin/music/menu-oric.ym"
            ], 
            "chunks": [
                "projects/goblin/chunks/scene01-oric.txt", 
                "projects/goblin/chunks/scene02-oric.txt", 
                "projects/goblin/chunks/scene03-oric.txt"
            ], 
            "musicMemory": "auto", 
            "chunkMemory": "$1800"
        }
    }
}