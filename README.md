# Personal Fork of Yabai

This is a fork of [FelixKratz/yabai](https://github.com/FelixFratz/yabai), which
is itself a fork of [koekeishiya/yabai](https://github.com/keokeishiya/yabai).

At the time I created this fork, Felix had the following personal patches for
yabai:

```
◆  felix.kratz@tu-dortmund.de 2024-03-15 02:07:22 32c0e4dc
│  document changes
◆  felix.kratz@tu-dortmund.de 2024-03-15 02:07:22 08025af5
│  auto padding prototype for large displays (yabai -m config auto_padding on)
◆  felix.kratz@tu-dortmund.de 2024-03-15 02:07:22 ab0f8db6
│  only allow one child at a time to zoom to parent node
◆  felix.kratz@tu-dortmund.de 2024-03-14 12:05:33 25bc294f
│  focus next window in node on window termination
◆  felix.kratz@tu-dortmund.de 2024-03-14 12:05:33 05b6d19b
│  focus closest window on application termination
◆  felix.kratz@tu-dortmund.de 2024-03-14 12:05:33 bf8a5a4a
│  zoomed windows occlude windows beneath in selection
```

Meanwhile, Yabai's not-yet-released developer branch had fixes necesary to get
its scripting addition working for Sequoia 15.1.  The scripting addition is
required to get nice space management with Yabai.

When rebasing, there were annoying conflicts with:
- 08025af5: auto padding prototype for large displays (yabai -m config auto_padding on)
- bf8a5a4a: zoomed windows occlude windows beneath in selection

Otherwise, the rest of the commits applied either perfectly cleanly, or with
minimal resolution.

I'm not too bothered by not having padding.  The zoomed windows fix sounds nice,
but due to how busy the conflict is, I wonder if the fix is already upstream.

Hopefully I don't have to maintain this fork for long as things work their way
either into Felix's fork, or even properly upstream.
