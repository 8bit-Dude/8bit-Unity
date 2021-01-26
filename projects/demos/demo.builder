{
    "format": "8bit-Unity Project", 
    "formatVersion": 2, 
    "general": {
        "disk": "demos", 
        "code": [
            "projects/demos/src/demo.c", 
            "projects/demos/src/demo-bitmap.c", 
            "projects/demos/src/demo-controls.c", 
            "projects/demos/src/demo-gfx.c", 
            "projects/demos/src/demo-hello.c", 
            "projects/demos/src/demo-network.c", 
            "projects/demos/src/demo-sprites.c"
        ], 
        "shared": [], 
        "charmap": []
    }, 
    "platform": {
        "Apple": {
            "spriteFrames": "64", 
            "spriteWidth": "7", 
            "spriteHeight": "5", 
            "bitmapDHR": [
                "projects/demos/bitmaps/banner-apple-dhr.png", 
                "projects/demos/bitmaps/stadium-apple-dhr.png"
            ], 
            "bitmapSHR": [
                "projects/demos/bitmaps/banner-apple-shr.png", 
                "projects/demos/bitmaps/stadium-apple-shr.png"
            ], 
            "charsetDHR": [], 
            "charsetSHR": [], 
            "spritesDHR": [
                "projects/demos/sprites/sprites-apple-dhr.png"
            ], 
            "spritesSHR": [
                "projects/demos/sprites/sprites-apple-shr.png"
            ], 
            "music": [
                "projects/demos/music/demo-apple.m"
            ], 
            "chunks": [], 
            "diskSize": "140KB", 
            "networkDriver": "IP65(TCP/UDP)"
        }, 
        "Atari": {
            "spriteFrames": "16", 
            "spriteWidth": "8", 
            "spriteHeight": "13", 
            "bitmap": [
                "projects/demos/bitmaps/banner-atari.png", 
                "projects/demos/bitmaps/stadium-atari.png"
            ], 
            "charset": [], 
            "sprites": [
                "projects/demos/sprites/sprites-atari.png"
            ], 
            "music": [
                "projects/demos/music/demo-atari.rmt"
            ], 
            "chunks": [], 
            "diskSize": "180KB", 
            "networkDriver": "IP65(TCP/UDP)"
        }, 
        "C64": {
            "spriteFrames": "16", 
            "spriteWidth": "12", 
            "spriteHeight": "21", 
            "bitmap": [
                "projects/demos/bitmaps/banner-c64.png", 
                "projects/demos/bitmaps/stadium-c64.png"
            ], 
            "charset": [], 
            "sprites": [
                "projects/demos/sprites/sprites-c64.png"
            ], 
            "music": [
                "projects/demos/music/demo-c64.sid"
            ], 
            "chunks": [], 
            "networkDriver": "IP65(TCP/UDP)"
        }, 
        "Lynx": {
            "spriteFrames": "16", 
            "spriteWidth": "7", 
            "spriteHeight": "9", 
            "musicMemory": "$0C00", 
            "sharedMemory": "$0400", 
            "bitmap": [
                "projects/demos/bitmaps/banner-lynx.png", 
                "projects/demos/bitmaps/stadium-lynx.png"
            ], 
            "charset": [], 
            "sprites": [
                "projects/demos/sprites/sprites-lynx.png"
            ], 
            "music": [
                "projects/demos/music/demo-lynx.asm"
            ], 
            "chunks": []
        }, 
        "Oric": {
            "spriteFrames": "16", 
            "spriteWidth": "12", 
            "spriteHeight": "6", 
            "dithering": "0.2", 
            "enforcedColors": " ", 
            "bitmap": [
                "projects/demos/bitmaps/banner-oric.png", 
                "projects/demos/bitmaps/stadium-oric.png"
            ], 
            "charset": [], 
            "sprites": [
                "projects/demos/sprites/sprites-oric.png"
            ], 
            "music": [
                "projects/demos/music/demo-oric.ym"
            ], 
            "chunks": []
        }
    }
}