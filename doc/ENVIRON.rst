Taisei Project – Environment Variables
======================================

.. contents::

Introduction
------------

Some of Taisei's more advanced configuration options are delegated to
`environment
variables <https://en.wikipedia.org/wiki/Environment_variable>`__. This
document attempts to describe them all.

Generally you don't need to set any of them. They are intended for
developers and advanced users only. Therefore, familiarity with the
concept is assumed.

In addition to the variables listed here, those processed by our runtime
dependencies (such as SDL) also take effect, unless stated otherwise.

Variables
---------

Virtual filesystem
~~~~~~~~~~~~~~~~~~

**TAISEI_RES_PATH**
   | Default: unset

   If set, overrides the default **resource directory** path. This is
   where Taisei looks for game data. The default path is platform and
   build specific:

   -  On **macOS**, it will be the ``Contents/Resources/data`` directory
      inside of the ``Taisei.app`` bundle.
   -  On **Linux**, **\*BSD**, and most other **Unix**-like systems
      (without -DRELATIVE), it will be ``$prefix/share/taisei``. This
      path is static and determined at compile time.
   -  On **Windows** and other platforms when built with -DRELATIVE, it
      will be the ``data`` directory relative to the executable (or to
      whatever ``SDL_GetBasePath`` returns on the given platform).

**TAISEI_STORAGE_PATH**
   | Default: unset

   If set, overrides the default **storage directory** path. This is
   where Taisei saves your configuration, progress, screenshots and
   replays. Taisei also loads custom data from the ``resources``
   subdirectory in there, if any, in addition to the stock assets. The
   custom resources shadow the default ones if the names clash. The
   default path is platform specific, and is equivalent to the return
   value of ``SDL_GetPrefPath("", "taisei")``:

   -  On **Windows**, it's ``%APPDATA%\taisei``.
   -  On **macOS**, it's ``$HOME/Library/Application Support/taisei``.
   -  On **Linux**, **\*BSD**, and most other **Unix**-like systems,
      it's ``$XDG_DATA_HOME/taisei`` or ``$HOME/.local/share/taisei``.

Resources
~~~~~~~~~

**TAISEI_NOASYNC**
   | Default: ``0``

   If ``1``, disables asynchronous loading. Increases loading times, might
   slightly reduce CPU and memory usage during loads. Generally not
   recommended unless you encounter a race condition bug, in which case
   you should report it.

**TAISEI_NOUNLOAD**
   | Default: ``0``

   If ``1``, loaded resources are never unloaded. Increases memory usage,
   reduces filesystem reads and loading times over time.

**TAISEI_NOPRELOAD**
   | Default: ``0``

   If ``1``, disables preloading. All resources are only loaded as they
   are needed. Reduces loading times and memory usage, but may cause
   stuttering during gameplay.

**TAISEI_PRELOAD_REQUIRED**
   | Default: ``0``

   If ``1``, the game will crash with an error message when it attempts to
   use a resource that hasn't been previously preloaded. Useful for
   developers to debug missing preloads.

**TAISEI_PRELOAD_SHADERS**
   | Default: ``0``

   If ``1``, Taisei will load all shader programs at startup. This is mainly
   useful for developers to quickly ensure that none of them fail to compile.

Video and OpenGL
~~~~~~~~~~~~~~~~

**TAISEI_PREFER_SDL_VIDEODRIVERS**
   | Default: ``wayland,mir,cocoa,windows,x11``

   List of SDL video backends that Taisei will attempt to use, in the
   specified order, before falling back to SDL's default. Entries may be
   separated by spaces, commas, colons, and semicolons. This variable is
   ignored if ``SDL_VIDEODRIVER`` is set.

**TAISEI_VIDEO_DRIVER**
   | Default: unset
   | **Deprecated**

   Use ``SDL_VIDEODRIVER`` instead.

**TAISEI_RENDERER**
   | Default: ``gl33``

   Selects the rendering backend to use. Currently available options are:

      -  ``gl33``: the OpenGL 3.3 Core renderer
      -  ``gles30``: the OpenGL ES 3.0 renderer
      -  ``null``: the no-op renderer (nothing is displayed)

   Note that the actual subset of usable backends, as well as the default
   choice, can be controlled by build options. The ``gles30`` backend is not
   built by default.

**TAISEI_LIBGL**
   | Default: unset

   OpenGL library to load instead of the default. The value has a
   platform-specific meaning (it's passed to the equivalent of ``dlopen``).
   Takes precedence over ``SDL_OPENGL_LIBRARY`` if set. Has no effect if
   Taisei is linked to libgl (which is not recommended, because it's not
   portable).

**TAISEI_GL_DEBUG**
   | Default: ``0`` for release builds, ``1`` for debug builds

   Enables OpenGL debugging. A debug context will be requested, all OpenGL
   messages will be logged, and errors are fatal. Requires the ``KHR_debug``
   or ``ARB_debug_output`` extension.

**TAISEI_GL_EXT_OVERRIDES**
   | Default: unset

   Space-separated list of OpenGL extensions that are assumed to be
   supported, even if the driver says they aren't. Prefix an extension with
   ``-`` to invert this behaviour. Might be used to work around bugs in
   some weird/ancient/broken drivers, but your chances are slim. Note that
   this only affects code paths that actually test for the given extensions,
   not the actual OpenGL functionality. Some OpenGL implementations (such as
   Mesa) provide their own mechanisms for controlling extensions. You most
   likely want to use that instead.

**TAISEI_FRAMERATE_GRAPHS**
   | Default: ``0`` for release builds, ``1`` for debug builds

   If ``1``, framerate graphs will be drawn on the HUD.

**TAISEI_OBJPOOL_STATS**
   | Default: ``0`` for release builds, ``1`` for debug builds

   Displays some statistics about usage of in-game objects.

Timing
~~~~~~

**TAISEI_HIRES_TIMER**
   | Default: ``1``

   If ``1``, tries to use the system's high resolution timer to limit the
   game's framerate. Disabling this is not recommended; it will likely make
   Taisei run slower or faster than intended and the reported FPS will be
   less accurate.

**TAISEI_FRAMELIMITER_SLEEP**
   | Default: ``0``

   If over ``0``, tries to sleep this many milliseconds after every frame
   if it was processed quickly enough. This reduces CPU usage by having the
   game spend less time in a busy loop, but may hurt framerate stability if
   set too high, especially if the high resolution timer is disabled or
   not available.

**TAISEI_FRAMELIMITER_SLEEP_EXACT**
   | Default: ``1``

   If ``1``, the framerate limiter will either try to sleep the exact
   amount of time set in ``TAISEI_FRAMELIMITER_SLEEP``, or none at all.
   Mitigates the aforementioned framerate stability issues by effectively
   making ``TAISEI_FRAMELIMITER_SLEEP`` do nothing if the value is too high
   for your system.

**TAISEI_FRAMELIMITER_COMPENSATE**
   | Default: ``1``

   If ``1``, the framerate limiter may let frames finish earlier than
   normal after sudden frametime spikes. This achieves better timing
   accuracy, but may hurt fluidity if the framerate is too unstable.

**TAISEI_FRAMELIMITER_LOGIC_ONLY**
   | Default: ``0``
   | **Experimental**

   If ``1``, only the logic framerate will be capped; new rendering frames
   will be processed as quickly as possible, with no delay. This inherently
   desynchronizes logic and rendering frames, and therefore, some logic
   frames may be dropped if rendering is too slow. However, unlike with the
   synchronous mode, the game speed will remain roughly constant in those
   cases. ``TAISEI_FRAMELIMITER_SLEEP``, ``TAISEI_FRAMELIMITER_COMPENSATE``,
   and the ``frameskip`` setting have no effect in this mode.

Logging
~~~~~~~

Taisei's logging system currently has four basic levels and works by
dispatching messages to a few output handlers. Each handler has a level
filter, which is configured by a separate environment variable. All of
those variables work the same way: their value looks like an IRC mode
string, and represents a modification of the handler's default settings.
If this doesn't make sense, take a look at the *Examples* section.

The levels
^^^^^^^^^^

-  **Debug** (*d*) is the most verbose level. It contains random
   information about internal workings of the game and is disabled for
   release builds at source level.
-  **Info** (*i*) logs some events that are expected to occur during
   normal operation, for example when a spell is unlocked or a
   screenshot is taken.
-  **Warning** (*w*) alerts of non-critical errors, for example a
   missing optional resource, corrupted progress data, or failure to
   save a replay due to insufficient storage space or privileges.
-  **Fatal Error** (*e*) is an irrecoverable failure condition. Such an
   event most likely signifies a programming error or a broken
   installation. The game will immediately crash after writing a message
   with this log level. On some platforms, it will also display a
   graphical message box.
-  **All** (*a*): this is not a real log level, but a shortcut directive
   representing all possible log levels. See *Examples* for usage.

The variables
^^^^^^^^^^^^^

-  **TAISEI_LOGLVLS_CONSOLE**: controls what goes to the console, both
   ``stdout`` and ``stderr``. Defaults to *All* (``+a``). This is a
   master switch for the two variables below:

   -  **TAISEI_LOGLVLS_STDOUT**: controls what goes to standard output.
      Defaults to *Debug and Info* (``+di``).
   -  **TAISEI_LOGLVLS_STDERR**: controls what goes to standard error.
      Defaults to *Warning and Fatal Error* (``+we``).

-  **TAISEI_LOGLVLS_FILE**: controls what goes into the log file
   (``{storage directory}/log.txt``). Defaults to *All* (``+a``).

-  **TAISEI_LOGLVLS_BACKTRACE**: affects all outputs and controls which
   log levels produce a call stack trace upon printing a message. Only
   supported in Debug builds with glibc. Defaults to *Fatal Error*
   (``+e``).

Examples
^^^^^^^^

-  In release builds: print *Info* messages to stdout, in addition to
   *Warning*\ s and *Fatal Error*\ s as per default:

   .. code:: sh

       TAISEI_LOGLVLS_STDOUT=+i

-  In Debug builds: remove *Debug* and *Info* output from the console:

   .. code:: sh

       TAISEI_LOGLVLS_STDOUT=-di

   OR:

   .. code:: sh

       TAISEI_LOGLVLS_CONSOLE=-di

-  Don't save anything to the log file:

   .. code:: sh

       TAISEI_LOGLVLS_FILE=-a

-  Don't print anything to the console:

   .. code:: sh

       TAISEI_LOGLVLS_CONSOLE=-a

-  Don't save anything to the log file, except for *Warning*\ s:

   .. code:: sh

       TAISEI_LOGLVLS_FILE=-a+w

-  Print everything except *Debug* to ``stderr``, nothing to ``stdout``:

   .. code:: sh

       TAISEI_LOGLVLS_STDOUT=-a
       TAISEI_LOGLVLS_STDERR=+a-d

-  In Debug builds: produce a stack trace for every *Warning* message,
   in addition to *Fatal Error*\ s as per default:

   .. code:: sh

       TAISEI_LOGLVLS_BACKTRACE=+w
