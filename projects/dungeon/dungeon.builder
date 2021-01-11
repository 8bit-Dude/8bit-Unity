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
        ]
    }, 
    "platform": {
        "Apple": {
            "spriteFrames": "32", 
            "spriteWidth": "7", 
            "spriteHeight": "16", 
            "bitmapDHR": [
                "projects/dungeon/bitmaps/menu-apple.png"
            ], 
            "spritesDHR": [
                "projects/dungeon/sprites/sprites-apple.png"
            ], 
            "music": [
                "projects/dungeon/music/dungeon-apple.m"
            ], 
            "chunks": [], 
            "charsetDHR": [
                "projects/dungeon/charsets/quedex-apple.png"
            ], 
            "diskSize": "140KB"
        }, 
        "Atari": {
            "spriteFrames": "32", 
            "spriteWidth": "8", 
            "spriteHeight": "18", 
            "bitmap": [
                "projects/dungeon/bitmaps/menu-atari.png"
            ], 
            "sprites": [
                "projects/dungeon/sprites/sprites-atari.png"
            ], 
            "music": [
                "projects/dungeon/music/dungeon-atari.rmt"
            ], 
            "chunks": [], 
            "charset": [
                "projects/dungeon/charsets/quedex-atari.png"
            ], 
            "noText": true, 
            "diskSize": "180KB"
        }, 
        "C64": {
            "spriteFrames": "32", 
            "spriteWidth": "12", 
            "spriteHeight": "21", 
            "bitmap": [
                "projects/dungeon/bitmaps/menu-c64.png"
            ], 
            "sprites": [
                "projects/dungeon/sprites/sprites-c64.png"
            ], 
            "music": [
                "projects/dungeon/music/dungeon-c64.sid"
            ], 
            "chunks": [], 
            "charset": [
                "projects/dungeon/charsets/quedex-c64.png"
            ]
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
            "sprites": [
                "projects/dungeon/sprites/sprites-lynx.png"
            ], 
            "music": [
                "projects/dungeon/music/dungeon-lynx.asm"
            ], 
            "chunks": [], 
            "charset": [
                "projects/dungeon/charsets/quedex-lynx.png"
            ]
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
            "sprites": [
                "projects/dungeon/sprites/sprites-oric.png"
            ], 
            "music": [
                "projects/dungeon/music/dungeon-oric.ym"
            ], 
            "chunks": [], 
            "charset": [
                "projects/dungeon/charsets/quedex-oric.png"
            ]
        }
    }
}