# ARIA

## Introduction

The Aria notification bubble is a highly customizable program wherein a user is
able to configure settings such as:
- Title of the notification.
- Body of the notification.
- Icon to display next to the text.
- Amount of time to display the notification.
- X-coordinate of where to put the notification on the screen.
- Y-coordinate of where to put the notification on the screen.
- Width of the notification.
- Height of the notification.
- Gravity of the notification.
- Opacity of the notification.
- Background color.
- Foreground color.
- Curvature to give corners of the notification bubble.
- Margin all around the notification.
- Top margin of the notification.
- Bottom margin of the notification.
- Left margin of the notification.
- Right margin of the notification.
- Spacing between the icon and notification bubble text.
- Font to display text in.
- Size of title text.
- Size of body text.

## Example

A list of examples will be shown below, however, if you have any questions, use
the *--help* option.

To fully test out the majority of the options that Aria possesses, you can run:
```
make test
```

If you wanted to stick with simpler examples, one of the simplest ways to use
Aria is to display text in the title and body. This can be done with the
following arguments in the command line:
```
./aria -t "Title" -b "Body"
```

Aria can take markup text, however, so if you want the title in bold, you can
simply add html bold tags.
```
./aria -t "<b>Important</b>" -b "Here is the message."
```

You can also display an icon next to the text.
```
./aria -t "<b>Portugal. The Man</b>" -b "Feel It Still" -i "/path/to/icon.jpg"
```

## Configure

To configure the notification bubble, you can either use the command line
options, or, you can modify the file under *~/.local/share/aria*. Running the
following command will add the configuration file to the aforementioned
directory:
```
make configure
```

This is done by default, when you run *make*.

## Install

To install the notification bubble to your system, run:
```
make
make install
```

The first command will compile the source code and copy the configuration file
to your home. The second command installs the program to your system.

## Uninstall

To uninstall the notification bubbble, run:
```
make uninstall
```

