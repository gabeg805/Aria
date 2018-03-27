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
the _--help_ option.

One of the simplest ways to use Aria is to display text in the title and
body. This can be done with the following arguments in the command line:
```
./aria -t "Title" -b "Body"
```

Aria can take markup text, however, so if you want the title in bold, you can
simply add _<b>...</b>_ bold tags.
```
./aria -t "<b>Important</b>" -b "Here is the message."
```

You can also display an icon next to the text.
```
./aria -t "<b>Portugal. The Man</b>" -b "Feel It Still" -i "/path/to/icon.jpg"
```


## Install


## Uninstall

