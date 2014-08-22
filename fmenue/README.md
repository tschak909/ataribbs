fmenue
======

fmenue is a file menu editor. Intended to build files to be used by the filemenu module.

Each file contains a list of entries:

* An item letter to be used to engage the item when using the file menu.
* An item path. The file to be shown, sans extension (which is added depending on terminal and availability)
* An item description. 

usage
=====

To use fmenue. Provide the filemenu file to edit as the parameter. By convention, filemenu files end in .FMU

 D1: FMENU BULLETIN.FMU

If you do not provide a filename, a usage example will be printed to the screen, and the program will exit.

If the file does not exist, fmenue will ask to create it. If it does exist, fmenue will load its entries, and fill in the editor appropriately

editing
=======

To edit an entry, select its letter or number. This will be the letter or number that a user uses to select a file to view.

You may then provide the appropriate information in the prompts that follow. Once done, you will return to the editor to edit more entries.

a note about filenames
======================

Filenames may point to any valid path or disk drive. e.g.

* BULLETIN
* D1:BULLETIN
* D1:FMU>BULLETIN
* D1:FMU\BULLETIN

The only constraint being that there is no file extension, as this is determined automatically by the BBS, depending on terminal and file availability.

More documentation to come, but this is just preliminary
-thom 
