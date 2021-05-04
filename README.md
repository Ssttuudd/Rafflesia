# Rafflesia
This is a bot for Lineage 2 Classic. It has been tested on L2 Classic Club and L2 NA off, but isn't maintained anymore. Feature requests and help regarding compilation or configuration will be silently ignored. Pull requests will be answered.

Walk away if you are looking for a production ready tool. You will need C++ and reverse engineering knowledge as well as a bit of time in order to use this code. I decided to share this mostly to help people to build their own tool.

# Features
- External UI with multi-account support
- Map with npcs, players and dropped items
- Players and npcs names, health, mana...
- Inclusion and exclusion polygons, as well as a mob filter list are available to filter targets
- Packet viewer to help debug or implement new functionalities
- Controlled player can target, move (using server's pathfinding), use actions and skills (spoil and sweep are implemented)
- A task system controls the bot's actions and an xml parser is included to write AI scripts in a declarative manner

# Technical stuffs
## Tools
- Qt
- Qt tools for VS
- VS 2019

## Dependencies
The following dependencies are used as submodules
- Minhook: https://github.com/TsudaKageyu/minhook.git
- PugiXml: https://github.com/zeux/pugixml.git
- fast-cpp-csv-parser: https://github.com/ben-strasser/fast-cpp-csv-parser.git

## Project hierarchy
```
- Rafflesia     // The external executable
  - Game        // Representation of the game (data structure, network packet)
    - Network   // Packet declaration and parsing
  - Bot         // Things that do actions in the game
    - Task      // Task classes and conditions
  - Data        // Non code files
    - ai        // AI scripts
    - dat       // client files
    - Maps      // l2net map images
  - Qt          // All the Qt code and design files are here
  - Injection   // Handles the root dll injection into the game
  - third_party // Dependencies

- Root          // The injected dll

- Shared        // Statically linked code shared between Rafflesia and Root
```

# How to get started
## Reverse engineering and injection
First off, you'll need to fill in the offset values in Offset.h. Basically you need to locate the offset of the encrypt, decrpyt and sendPacket functions, which isn't that hard if you can break on WS2_32::send and WS2_32::recv. Check the value of SEND_PACKET_INFO as well while you're in WS2_32::send. 
/!\ Do not use ingame character movement to get those offset, the client use different functions for the movements.

Now that you have the offsets, you need something to inject the dll. This project doesn't inject the dll directly, it uses an injector that isn't provided here. Change the values in Injection/Injector.h and check the deleguateInjection function in Injection/Injector.cpp to see if the command line works with the injector you chose.

Check out the protocol version, if it's not 140, you'll have some problems. Most of the packets won't be much different, but some will. Use the packet viewer window and compare what you see with the parse functions Game/Network/Packet140.cpp. Add your own parsing functions and update the PacketHandler constructor to put them to good use.

## Starting the bot
The bot will not inject any client automatically, instead it'll show you the list of clients it detected and will allow you to inject them. Once it's done and the communication between the external bot and the injected dll is established, a new tab will appear. Each client will have his own tab.

In order to have a small dll and a better maintanability, the bot uses the network packets to get the informations from the game instead of reading its memory. For this reason, you'll need to have the bot linked with your client before you select your character. If you inject the dll while already ingame, going to the character selection screen and then back into the game will work too.

The dll will be unloaded anytime you disconnect the external bot. If you stop the external process (typically you break in debug and stop to fix something), the dll might not unload properly. In this case, you can use the VK_PAUSE key to order the dll to unload.

## Using the bot
I won't list all the stuff you can do in the UI, just click around and discover by yourself.
If you want to start farming, you'll need to take a look at the Data/ai folder. You'll find small scripts in there that show what you can do with the task system as of now. Loading scripts from the UI isn't implemented (but is trivial to achieve), you'll need to modify Bot::toggleStart to load the script you want for your character.


# Credits
- I took inspiration and code chunks from xarkes' work: https://github.com/xarkes/L2Bot
