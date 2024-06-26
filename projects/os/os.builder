{
    "format": "8bit-Unity Project", 
    "formatVersion": 2, 
    "general": {
        "disk": "os", 
        "code": [
            "projects/os/src/os.c", 
            "projects/os/src/chat.c", 
            "projects/os/src/files.c", 
            "projects/os/src/home.c", 
            "projects/os/src/image.c", 
            "projects/os/src/mouse.c", 
            "projects/os/src/music.c", 
            "projects/os/src/network.c"
        ], 
        "shared": [
            "projects/os/shared/readme.txt"
        ], 
        "charmap": [], 
        "tilesize": "None"
    }, 
    "platform": {
        "Apple": {
            "spriteFrames": "2", 
            "spriteWidth": "7", 
            "spriteHeight": "7", 
            "bitmapDHR": [
                "projects/os/bitmaps/hokusai-apple.png", 
                "projects/os/bitmaps/munch-apple.png", 
                "projects/os/bitmaps/picasso-apple.png", 
                "projects/os/bitmaps/vangogh-apple.png"
            ], 
            "bitmapSHR": [], 
            "charsetDHR": [], 
            "charsetSHR": [], 
            "spritesDHR": [
                "projects/os/sprites/sprites-apple.png"
            ], 
            "spritesSHR": [], 
            "music": [
                "projects/os/music/dungeon-apple.m", 
                "projects/os/music/goblin-apple.m", 
                "projects/os/music/speednik-apple.m"
            ], 
            "chunks": [
                "projects/os/chunks/icons-apple.txt"
            ], 
            "diskSize": "140KB", 
            "crunchAssets": "No", 
            "musicMemory": "auto", 
            "chunkMemory": "$0500", 
            "assetFilter": "*", 
            "network8bitHub": true, 
            "networkUthernet": true, 
            "networkProtocols": "TCP/UDP"
        }, 
        "Atari": {
            "spriteFrames": "2", 
            "spriteWidth": "8", 
            "spriteHeight": "6", 
            "bitmap": [
                "projects/os/bitmaps/hokusai-atari.png", 
                "projects/os/bitmaps/munch-atari.png", 
                "projects/os/bitmaps/picasso-atari.png", 
                "projects/os/bitmaps/vangogh-atari.png"
            ], 
            "charset": [], 
            "sprites": [
                "projects/os/sprites/sprites-atari.png"
            ], 
            "music": [
                "projects/os/music/bazalt-atari.rmt", 
                "projects/os/music/nirvana-atari.rmt"
            ], 
            "chunks": [
                "projects/os/chunks/icons-atari.txt"
            ], 
            "diskSize": "180KB", 
            "crunchAssets": "No", 
            "musicMemory": "$0800", 
            "chunkMemory": "$0C00", 
            "assetFilter": "*", 
            "network8bitHub": true, 
            "networkDragonCart": true, 
            "networkFujinet": true, 
            "networkProtocols": "TCP/UDP"
        }, 
        "C64": {
            "spriteFrames": "2", 
            "spriteWidth": "12", 
            "spriteHeight": "21", 
            "bitmap": [
                "projects/os/bitmaps/hokusai-c64.png", 
                "projects/os/bitmaps/munch-c64.png", 
                "projects/os/bitmaps/picasso-c64.png", 
                "projects/os/bitmaps/vangogh-c64.png"
            ], 
            "charset": [], 
            "charsetColors": " ", 
            "sprites": [
                "projects/os/sprites/sprites-c64.png"
            ], 
            "music": [
                "projects/os/music/berger-c64.sid", 
                "projects/os/music/ryu-c64.sid"
            ], 
            "chunks": [
                "projects/os/chunks/icons-c64.txt"
            ], 
            "crunchAssets": "No", 
            "musicMemory": "$4000", 
            "chunkMemory": "$0A00", 
            "assetFilter": "*", 
            "network8bitHub": true, 
            "networkRRNet": true, 
            "networkProtocols": "TCP/UDP"
        }, 
        "Lynx": {
            "spriteFrames": "2", 
            "spriteWidth": "9", 
            "spriteHeight": "13", 
            "bitmap": [
                "projects/os/bitmaps/hokusai-lynx.png", 
                "projects/os/bitmaps/munch-lynx.png", 
                "projects/os/bitmaps/picasso-lynx.png", 
                "projects/os/bitmaps/vangogh-lynx.png"
            ], 
            "charset": [], 
            "sprites": [
                "projects/os/sprites/sprites-lynx.png"
            ], 
            "music": [
                "projects/os/music/chase-lynx.asm", 
                "projects/os/music/driven-lynx.asm", 
                "projects/os/music/speednik-lynx.asm", 
                "projects/os/music/stroll-lynx.asm", 
                "projects/os/music/whirlwnd-lynx.asm"
            ], 
            "chunks": [
                "projects/os/chunks/icons-lynx.txt"
            ], 
            "musicMemory": "$0C00", 
            "chunkMemory": "$0800", 
            "sharedMemory": "$0800", 
            "assetFilter": "*"
        }, 
        "NES": {
            "bitmapTiles": "176", 
            "spriteFrames": "2", 
            "spriteWidth": "16", 
            "spriteHeight": "16", 
            "bitmap": [
                "projects/os/bitmaps/bench-nes.png", 
                "projects/os/bitmaps/hokusai-nes.png", 
                "projects/os/bitmaps/munch-nes.png", 
                "projects/os/bitmaps/picasso-nes.png", 
                "projects/os/bitmaps/vangogh-nes.png"
            ], 
            "charset": [], 
            "sprites": [
                "projects/os/sprites/sprites-nes.png"
            ], 
            "music": [
                "projects/os/music/goblin-nes.txt", 
                "projects/os/music/speednik-nes.txt"
            ], 
            "chunks": [
                "projects/os/chunks/icons-nes.txt"
            ], 
            "musicMemory": "$4000", 
            "chunkMemory": "$0200", 
            "assetFilter": "*"
        }, 
        "Oric": {
            "spriteFrames": "2", 
            "spriteWidth": "12", 
            "spriteHeight": "8", 
            "dithering": "0.2", 
            "bitmap": [
                "projects/os/bitmaps/hokusai-oric.png", 
                "projects/os/bitmaps/munch-oric.png", 
                "projects/os/bitmaps/picasso-oric.png", 
                "projects/os/bitmaps/vangogh-oric.png"
            ], 
            "charset": [], 
            "sprites": [
                "projects/os/sprites/sprites-oric.png"
            ], 
            "music": [
                "projects/os/music/demonia-oric.ym", 
                "projects/os/music/grenade-oric.ym", 
                "projects/os/music/speednik-oric.ym"
            ], 
            "chunks": [
                "projects/os/chunks/icons-oric.txt"
            ], 
            "musicMemory": "auto", 
            "chunkMemory": "$0700", 
            "assetFilter": "*"
        }
    }
}