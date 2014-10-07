Lib Wayland Security Modules
============================

LibWSM is a framework that supports security decision making on [Wayland](http://wayland.freedesktop.org/)-based graphic stacks. It implements methods for expressing security decisions on privileged interfaces and an interface for security engineers to write backends. It is shipped with a default backend that allows per-user and per-application security policies. Those policies can be extended without limits to support per-compositor decisions, compositor-specific capabilities and even custom security decisions.

This project was designed and developed by [Martin Peres](http://www.mupuf.org/contact/mupuf.html) and [Steve Dodier-Lazaro](http://www.mupuf.org/contact/sidi.html). It is maintained by Martin Peres. The code is [hosted on GitHub](https://github.com/mupuf/libwsm/).

Sources of policy
-----------------
When a policy is not set for a client;capability;object set, libWSM will implicitly deny actions. If policies are available, the default backend will take them into account in the following order from top to bottom:

 - The policy that matches the client's executable and uid
 - The system-wide policy that matches the client's executable
 - The generic policy that matches the client's uid
 - The system-wide generic policy for all clients
 - Implicit Deny if no policy is available

Backends can give four recommendations by default:

 - **Explicit allow:** the policy allows the action to be performed. It is recommended to allow the action.
 - **Implicit allow:** there probably is nothing wrong, but attacks could be carried out. It is recommended to provide feedback to the user about the action being performed.
 - **Implicit allow:** the requesting app has not obtained the permission to perform the action. It is recommended to seek an alternative way to provide permission if desired (e.g., trusted UIs or permission prompt)
 - **Explicit deny:** the action has been explicitly written as denied in the policy. It is recommended to follow the policy and deny the action without alternative.

Other permissions can be defined. As a rule of thumb, when a compositor does not recognise a permission, *‘implicit deny’* should be followed.

Capabilities related to the Graphic Stack
-----------------------------------------
The following capabilities are made available. `default:` indicates what the default policy shipped with the default backend states.

**WSM_SCREENSHOT** `default: implicit-deny`
Ability to take a still screenshot of the whole screen

**WSM_SCREENSHARING** `default: implicit-deny`
Ability to record the screen continuously

**WSM_VIRTUAL_KEYBOARD** `explicit-allow/inject-only/filter-only/implicit-allow/implicit-deny/explicit-deny` `default: implicit-deny`
Ability to inject or filter input on the keyboard

**WSM_VIRTUAL_POINTING** `default: implicit-deny`
Ability to modify the position of the pointer and simulate clicks

**WSM_GLOBAL_KEYBOARD_SEQUENCE** `object: key sequence` `default: implicit-deny`
Ability to receive global keyboard sequences when not on focus

**WSM_FORWARD_RESERVED_KEYBOARD_SEQUENCE** `object: key sequence` `default: implicit-deny`
Ability to receive reserved keyboard sequences instead of compositor when on focus

**WSM_FULLSCREEN** `default: implicit-allow`
Ability to use the entire screen

**WSM_CLIPBOARD_COPY** `default: implicit-allow`
Ability to copy to the clipboard

**WSM_CLIPBOARD_PASTE** `default: implicit-deny`
Ability to paste from the clipboard

**WSM_RAISE_FOCUS** `default: explicit-allow`
Ability to raise the window and grab focus programmatically

Possible future capabilities not related to the Graphic Stack
-------------------------------------------------------------
**DSM_RECORD_VIDEO** `default: implicit-deny`
Ability to read data on video capture devices

**DSM_RECORD_AUDIO** `default: implicit-deny`
Ability to read data on audio capture devices

**DSM_USE_PASSWORD_STORE** `default: implicit-allow`
Ability to use password store APIs

**DSM_PRIVILEGED_HEADLESS** `default: implicit-allow`
Ability to perform privileged actions without having a GUI

**DSM_SESSION_LOCKER** `default: explicit-deny`
Ability to lock the user session

**DSM_AUTHENTICATION_UI** `default: explicit-deny`
Ability to act as an authentication UI

**DSM_PERMISSION_UI** `default: explicit-deny`
Ability to act as a permission UI

