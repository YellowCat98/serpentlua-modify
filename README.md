# Modify

- A hooking utility for [SerpentLua](https://github.com/YellowCat98/SerpentLua).

## Classes supported:
- MenuLayer
- CreatorLayer
- LoadingLayer
- GameManager
- GameStatsManager

## Usage:
1. Create the hook
> ```lua
> Modify.createHook(hookID, className, functionName, detour)
> ```
- hookID: an ID linked to the hook.
- className: The class you want to hook.
- functionName: The function of that class you want to hook.
- detour: What to replace the function with.
2. Detour
- The detour is the last argument of the createHook function. it is written in this form:
> ```lua
> function(self, args)
>     --Code goes here
> end
> ```
- (yes just a lambda)
- If the function you want to hook is not declared static, omit the `self` argument. Otherwise it must be kept.
- `self` simply corresponds to the instance of the class that is calling the hook.
- `args` is a placeholder for the arguments the function takes.
- If you are using a plugin alongside Modify that binds this class to lua, you may access its methods.
3. Calling original.
- Original is simply the original function, this is useful for extending on a function instead of replacing what it does.
- Modify declares an `original` function within the detour's scope. it takes the same arguments that the function expects. you can simply call it through `original(self, args)`.
4. Applying the hook
- `createHook` only registers there is a hook that can be applied.
- To apply it, use the `applyHook` function. Which simply takes a single argument.
> ```lua
> Modify.applyHook(hookID)
> ```
- hookID: the hook that you want to be applied.
- `applyHook` must be called for every hook that you create in order for it to take effect.

## Possible Question:
- Q: Why does Geode show in the console that a hook was applied even though the script doesn't apply it?
- A: The way Modify works internally, it uses one hook for every function that gets hooked. And simply calls all the functions that hook it. This is in order to preserve the original call chain. So that the original function will not get called twice because of two scripts that call it twice.