{
    "format": "8bit-Unity Project", 
    "formatVersion": 2, 
    "general": {
        "disk": "demos", 
        "code": [
            "projects/demos/src/demo.c", 
            "projects/demos/src/demo-bitmap.c", 
            "projects/demos/src/demo-charmap.c", 
            "projects/demos/src/demo-controls.c", 
            "projects/demos/src/demo-gfx.c", 
            "projects/demos/src/demo-hello.c", 
            "projects/demos/src/demo-network.c", 
            "projects/demos/src/demo-sprites.c"
        ], 
        "shared": [], 
        "charmap": [
            "projects/demos/charmaps/level1.map", 
            "projects/demos/charmaps/level1.tls"
        ], 
        "tilesize": "2x2"
    }, 
    "platform": {
        "Apple": {
            "spriteFrames": "64", 
            "spriteWidth": "7", 
            "spriteHeight": "6", 
            "bitmapDHR": [
                "projects/demos/bitmaps/banner-apple-dhr.png", 
                "projects/demos/bitmaps/stadium-apple-dhr.png"
            ], 
            "bitmapSHR": [
                "projects/demos/bitmaps/banner-apple-shr.png", 
                "projects/demos/bitmaps/stadium-apple-shr.png"
            ], 
            "charsetDHR": [
                "projects/demos/charsets/quedex-apple.png"
            ], 
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
            "crunchAssets": "No", 
            "musicMemory": "auto", 
            "chunkMemory": "$0400", 
            "assetFilter": "*", 
            "network8bitHub": true, 
            "networkUthernet": true, 
            "networkProtocols": "TCP/UDP"
        }, 
        "Atari": {
            "spriteFrames": "18", 
            "spriteWidth": "8", 
            "spriteHeight": "10", 
            "bitmap": [
                "projects/demos/bitmaps/banner-atari.png", 
                "projects/demos/bitmaps/stadium-atari.png"
            ], 
            "charset": [
                "projects/demos/charsets/quedex-atari.png"
            ], 
            "sprites": [
                "projects/demos/sprites/sprites-atari.png"
            ], 
            "music": [
                "projects/demos/music/demo-atari.rmt"
            ], 
            "chunks": [], 
            "diskSize": "180KB", 
            "crunchAssets": "No", 
            "musicMemory": "$0800", 
            "chunkMemory": "$0900", 
            "assetFilter": "*", 
            "network8bitHub": true, 
            "networkDragonCart": true, 
            "networkFujinet": true, 
            "networkProtocols": "TCP/UDP"
        }, 
        "C64": {
            "spriteFrames": "16", 
            "spriteWidth": "12", 
            "spriteHeight": "21", 
            "bitmap": [
                "projects/demos/bitmaps/banner-c64.png", 
                "projects/demos/bitmaps/stadium-c64.png"
            ], 
            "charset": [
                "projects/demos/charsets/quedex-c64.png"
            ], 
            "charsetColors": " ", 
            "sprites": [
                "projects/demos/sprites/sprites-c64.png"
            ], 
            "music": [
                "projects/demos/music/demo-c64.sid"
            ], 
            "chunks": [], 
            "crunchAssets": "No", 
            "musicMemory": "$4000", 
            "chunkMemory": "$0800", 
            "assetFilter": "*", 
            "network8bitHub": true, 
            "networkRRNet": true, 
            "networkUltimate": true, 
            "networkProtocols": "TCP/UDP"
        }, 
        "Lynx": {
            "spriteFrames": "16", 
            "spriteWidth": "8", 
            "spriteHeight": "9", 
            "bitmap": [
                "projects/demos/bitmaps/banner-lynx.png", 
                "projects/demos/bitmaps/stadium-lynx.png"
            ], 
            "charset": [
                "projects/demos/charsets/quedex-lynx.png"
            ], 
            "sprites": [
                "projects/demos/sprites/sprites-lynx.png"
            ], 
            "music": [
                "projects/demos/music/demo-lynx.asm"
            ], 
            "chunks": [], 
            "musicMemory": "$0800", 
            "chunkMemory": "$0400", 
            "sharedMemory": "$0000", 
            "assetFilter": "*"
        }, 
        "NES": {
            "bitmapTiles": "160", 
            "spriteFrames": "16", 
            "spriteWidth": "16", 
            "spriteHeight": "16", 
            "bitmap": [
                "projects/demos/bitmaps/banner-nes.png", 
                "projects/demos/bitmaps/stadium-nes.png"
            ], 
            "charset": [
                "projects/demos/charsets/quedex-nes.png"
            ], 
            "sprites": [
                "projects/demos/sprites/sprites-nes.png"
            ], 
            "music": [
                "projects/demos/music/demo-nes.txt"
            ], 
            "chunks": [], 
            "musicMemory": "$4000", 
            "chunkMemory": "$0200", 
            "assetFilter": "*"
        }, 
        "Oric": {
            "spriteFrames": "16", 
            "spriteWidth": "12", 
            "spriteHeight": "6", 
            "dithering": "0.2", 
            "leadColors": " ", 
            "bitmap": [
                "projects/demos/bitmaps/banner-oric.png", 
                "projects/demos/bitmaps/stadium-oric.png"
            ], 
            "charset": [
                "projects/demos/charsets/quedex-oric.png"
            ], 
            "sprites": [
                "projects/demos/sprites/sprites-oric.png"
            ], 
            "music": [
                "projects/demos/music/demo-oric.ym"
            ], 
            "chunks": [], 
            "musicMemory": "auto", 
            "chunkMemory": "$0500", 
            "assetFilter": "*"
        }
    }
}