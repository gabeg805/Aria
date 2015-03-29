===========
What is it?
===========

A customizable notification bubble that gives the user flexibility to choose:
    
    - Title text and size
    - Body text and size
    - Notification x/y-position
    - Notification opacity
    - Notification margin size
    - Amount of time the notification will display for
    - Notification font
    - Notification font color
    - Notification background color



=============
Documentation
=============

The notification bubble documentation is included in the header of the source
code. As a quick example, to create a very simple notification bubble, execute the
following command in the command line:

$ ./noti -t "Title" -b "Body"



============
Installation
============

Recompile the binary and update your PATH environment variable in your shell rc file
with:
    
    $ rm noti
    $ make
    $ export PATH="${PATH}":"/PATH/TO/PROGRAM/noti"

Now the program is ready for use!



========
Contacts
========

If you have any problems, feel free to email me at 'gabeg@bu.edu'.



==================
Potential Problems
==================

Unknown



=====
To-Do
=====

- Set limits on the default variable values

- Be able to handle multiple bubbles being opened up at one time.

- Create a .config file to hold default values.
