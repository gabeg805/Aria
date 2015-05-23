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
    
    $ ./aria -t "Title" -b "Body"



============
Installation
============

Recompile the binary and update your PATH environment variable in your shell rc file
with:
    
    $ export PATH="${PATH}":"/PATH/TO/PROGRAM/aria"

Now the program is ready for use!



========
Contacts
========

If you have any problems, feel free to email me at 'gabeg@bu.edu'.



==================
Potential Problems
==================

- Lags a little bit on very first execution of program



=====
To-Do
=====

- Set limits on the default variable values

- Be able to handle multiple bubbles being opened up at one time.
