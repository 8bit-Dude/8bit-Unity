{
    "format": "8bit-Unity Project", 
    "formatVersion": 2, 
    "general": {
        "disk": "dungeon", 
        "code": [
            "projects/dungeon/src/dungeon.c", 
            "projects/dungeon/src/actors.c", 
            "projects/dungeon/src/interface.c", 
            "projects/dungeon/src/player.c"
        ], 
        "shared": [
            "projects/dungeon/charmaps/level1.act"
        ], 
        "charmap": [
            "projects/dungeon/charmaps/level1.map", 
            "projects/dungeon/charmaps/level1.tls"
        ], 
        "tilesize": "2x2", 
        "customVBI": "No"
    }, 
    "platform": {
        "Apple": {
            "spriteFrames": "32", 
            "spriteWidth": "7", 
            "spriteHeight": "16", 
            "bitmapDHR": [
                "projects/dungeon/bitmaps/menu-apple.png"
            ], 
            "bitmapSHR": [], 
            "charsetDHR": [
                "projects/dungeon/charsets/quedex-apple.png"
            ], 
            "charsetSHR": [], 
            "spritesDHR": [
                "projects/dungeon/sprites/sprites-apple.png"
            ], 
            "spritesSHR": [], 
            "music": [
                "projects/dungeon/music/dungeon-apple.m"
            ], 
            "chunks": [], 
            "diskSize": "140KB", 
            "crunchAssets": "No", 
            "networkDriver": "IP65(TCP/UDP)"
        }, 
        "Atari": {
            "spriteFrames": "32", 
            "spriteWidth": "8", 
            "spriteHeight": "18", 
            "bitmap": [
                "projects/dungeon/bitmaps/menu-atari.png"
            ], 
            "charset": [
                "projects/dungeon/charsets/quedex-atari.png"
            ], 
            "sprites": [
                "projects/dungeon/sprites/sprites-atari.png"
            ], 
            "music": [
                "projects/dungeon/music/dungeon-atari.rmt"
            ], 
            "chunks": [], 
            "diskSize": "180KB", 
            "crunchAssets": "No", 
            "networkDriver": "IP65(TCP/UDP)"
        }, 
        "C64": {
            "spriteFrames": "32", 
            "spriteWidth": "12", 
            "spriteHeight": "21", 
            "bitmap": [
                "projects/dungeon/bitmaps/menu-c64.png"
            ], 
            "charset": [
                "projects/dungeon/charsets/quedex-c64.png"
            ], 
            "charsetColors": " ", 
            "sprites": [
                "projects/dungeon/sprites/sprites-c64.png"
            ], 
            "music": [
                "projects/dungeon/music/dungeon-c64.sid"
            ], 
            "chunks": [], 
            "crunchAssets": "No", 
            "networkDriver": "IP65(TCP/UDP)"
        }, 
        "Lynx": {
            "spriteFrames": "32", 
            "spriteWidth": "8", 
            "spriteHeight": "12", 
            "musicMemory": "$0C00", 
            "sharedMemory": "$0400", 
            "bitmap": [
                "projects/dungeon/bitmaps/menu-lynx.png"
            ], 
            "charset": [
                "projects/dungeon/charsets/quedex-lynx.png"
            ], 
            "sprites": [
                "projects/dungeon/sprites/sprites-lynx.png"
            ], 
            "music": [
                "projects/dungeon/music/dungeon-lynx.asm"
            ], 
            "chunks": []
        }, 
        "NES": {
            "spriteFrames": "32", 
            "spriteWidth": "16", 
            "spriteHeight": "16", 
            "bitmap": [
                "projects/dungeon/bitmaps/menu-nes.png"
            ], 
            "charset": [
                "projects/dungeon/charsets/quedex-nes.png"
            ], 
            "sprites": [
                "projects/dungeon/sprites/sprites-nes.png"
            ], 
            "music": [], 
            "chunks": []
        }, 
        "Oric": {
            "spriteFrames": "32", 
            "spriteWidth": "12", 
            "spriteHeight": "17", 
            "dithering": "0.4", 
            "enforcedColors": " ", 
            "bitmap": [
                "projects/dungeon/bitmaps/menu-oric.png"
            ], 
            "charset": [
                "projects/dungeon/charsets/quedex-oric.png"
            ], 
            "sprites": [
                "projects/dungeon/sprites/sprites-oric.png"
            ], 
            "music": [
                "projects/dungeon/music/dungeon-oric.ym"
            ], 
            "chunks": []
        }
    }
}