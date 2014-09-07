mboarde
=======

mboarde is a message board menu editor. Intended to build boards to be used by the boardmenu module.

Each board contains a list of entries:

* An item letter to be used to engage the item when using the message board menu.
* An item path. The board to be shown, sans extension (which is added depending on terminal and availability)
* An item description. 

usage
=====

To use mboarde. Provide the boardmenu board to edit as the parameter. By convention, boardmenu boards end in .MMU

 D1: MBOARDE BULLETIN.MMU

If you do not provide a boardname, a usage example will be printed to the screen, and the program will exit.

If the board does not exist, mboarde will ask to create it. If it does exist, fmenue will load its entries, and fill in the editor appropriately

editing
=======

To edit an entry, select its letter or number. This will be the letter or number that a user uses to select a board to view.

You may then provide the appropriate information in the prompts that follow. Once done, you will return to the editor to edit more entries.

a note about boardnames
======================

Filenames may point to any valid path or disk drive. e.g.

* ATARI8 
* D1:ATARI8
* D1:MMU>ATARI8
* D1:MMU\ATARI8

The only constraint being that there is no board extension, as this is determined automatically by the BBS, depending on terminal and board availability.

More documentation to come, but this is just preliminary
-thom 
