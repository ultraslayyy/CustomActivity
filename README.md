# CustomActivity

A simple and easy to use Discord Custom Activity/Rich Presence Client made in Qt6.

## Usage

> [!NOTE]
> Application ID is an optional field for users who choose to use their own Discord Application ID instead of the default one.
> Later, an option for authentication using your own account may be added, for users who just don't want to use an Application ID.

To use, hit 'Connect' and simply enter in the details and hit 'Set Activity'. To remove it, press 'Clear Activity'. To quit, either press the exit button or right click on the app in the system tray (Windows) and hit 'Exit', since the app runs in the background when closed.

A basic key of what correlates to what part of the presence is the following legend:
![Discord Rich Presence Legend](https://mintcdn.com/discord/eCuA2RUd7jZAoO2Y/images/rich-presence/legend.png?w=840&fit=max&auto=format&n=eCuA2RUd7jZAoO2Y&q=85&s=05dc3c55d946a907cee88403262c3b27)

Although you can also experiment around and see what does work.

> [!IMPORTANT]
> Buttons are only visible to other users — you cannot see buttons on your own Rich Presence. To test that your buttons are working, use a second account or ask a friend to view your profile.

## Building yourself

> [!IMPORTANT]
> More informative steps may be provided later, I'm just too lazy rn

To build it yourself simply build it as a standard Qt app, no extra steps.
