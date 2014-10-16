Lib Wayland Security Modules
============================

LibWSM is a framework that supports security decision making on [Wayland](http://wayland.freedesktop.org/)-based graphic stacks. It implements methods for expressing security decisions on privileged interfaces and an interface for security engineers to write backends. It is shipped with a default backend that allows per-user and per-application security policies. Those policies can be extended without limits to support per-compositor decisions, compositor-specific capabilities and even custom security decisions.

This project was designed and developed by [Martin Peres](http://www.mupuf.org/contact/mupuf.html) and [Steve Dodier-Lazaro](http://www.mupuf.org/contact/sidi.html). It is maintained by Martin Peres. The code is [hosted on GitHub](https://github.com/mupuf/libwsm/).

Sources of policy
-----------------
When a policy is not set for a client;capability;object set, libWSM will soft-deny actions. If policies are available, the default backend will take them into account in the following order from top to bottom:

 - The policy that matches the client's executable and uid
 - The system-wide policy that matches the client's executable
 - The generic policy that matches the client's uid
 - The system-wide generic policy for all clients
 - Soft Deny if no policy is available

Backends can give four recommendations by default:

 - **Allow:** the policy allows the action to be performed. It is recommended to allow the action.
 - **Soft allow:** there probably is nothing wrong, but attacks could be carried out. It is recommended to provide feedback to the user about the action being performed.
 - **Soft allow:** the requesting app has not obtained the permission to perform the action. It is recommended to seek an alternative way to provide permission if desired (e.g., trusted UIs or permission prompt)
 - **Deny:** the action has been explicitly written as denied in the policy. It is recommended to follow the policy and deny the action without alternative.

Other permissions can be defined. As a rule of thumb, when a compositor does not recognise a permission, *‘Deny’* should be followed.

Capabilities related to the Graphic Stack
-----------------------------------------
The following capabilities are made available. `default:` indicates what the default policy shipped with the default backend states.

**WSM_SCREENSHOT** `default: soft-deny`
Ability to take a still screenshot of the whole screen

**WSM_SCREENSHARING** `default: soft-deny`
Ability to record the screen continuously

**WSM_VIRTUAL_KEYBOARD** `allow/inject-only/filter-only/soft-allow/soft-deny/deny` `default: soft-deny`
Ability to inject or filter input on the keyboard

**WSM_VIRTUAL_POINTING** `default: soft-deny`
Ability to modify the position of the pointer and simulate clicks

**WSM_GLOBAL_KEYBOARD_SEQUENCE** `object: key sequence` `default: soft-deny`
Ability to receive global keyboard sequences when not on focus

**WSM_FORWARD_RESERVED_KEYBOARD_SEQUENCE** `object: key sequence` `default: soft-deny`
Ability to receive reserved keyboard sequences instead of compositor when on focus

**WSM_FULLSCREEN** `default: soft-allow`
Ability to use the entire screen

**WSM_CLIPBOARD_COPY** `default: allow`
Ability to copy to the clipboard

**WSM_CLIPBOARD_PASTE** `default: soft-deny`
Ability to paste from the clipboard

**WSM_RAISE_FOCUS** `default: soft-allow`
Ability to raise the window and grab focus programmatically

**WSM_NOTIFICATION_API** `default: soft-allow`
Ability to use the libnotify API to raise notifications

**WSM_CUSTOM_NOTIFICATION_API** `default: soft-deny`
Ability to build custom notification UIs and have them displayed (i.e., raised and positioned for a set duration) by the compositor

Possible future capabilities not related to the Graphic Stack
-------------------------------------------------------------
**DSM_RECORD_VIDEO** `default: soft-deny`
Ability to read data on video capture devices

**DSM_RECORD_AUDIO** `default: soft-deny`
Ability to read data on audio capture devices

**DSM_USE_PASSWORD_STORE** `default: soft-allow`
Ability to use password store APIs

**DSM_PRIVILEGED_HEADLESS** `default: soft-allow`
Ability to perform privileged actions without having a GUI

**DSM_SESSION_LOCKER** `default: deny`
Ability to lock the user session

**DSM_AUTHENTICATION_UI** `default: deny`
Ability to act as an authentication UI

**DSM_PERMISSION_UI** `default: deny`
Ability to act as a permission UI


Things LibWSM is currently bad at
---------------------------------

**Expressing policies that apply only in certain modes of a Wayland compositor** This won't be fixed unless a compositor developer can convince us that their compositor implements different desktops that their users conceive as entirely distinct (e.g., KDE?)

**Adding options to capabilities** e.g., on the notification API, different decisions for focus raising, custom UI, actions, UI positioning... Support is desirable but will complicate policy a great deal. Feedback is wanted from DE developers

