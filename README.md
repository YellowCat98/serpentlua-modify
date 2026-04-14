# Modify

- A hooking utility for [SerpentLua](https://github.com/YellowCat98/SerpentLua).

## Classes supported:
- MenuLayer
- CreatorLayer
- LoadingLayer
- GameManager
- GameStatsManager

## Usage:
(this is step zero because i dont want to go through incrementing all the steps numbers to make this one step 1)
0. As you should with every plugin you wish to use, make sure that it is added to the metadata of the script. and that you have retrieved the plugin's module through `require("yellowcat98.modify")`.
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

## Special Behavior
- These are things that Modify does for certain functions.
### Overloaded Functions
- Overloads are functions with the same name, but take different parameters.
- Modify handles these by appending a __num to the function's name.
- Example:
> ```lua
> Modify.createHook("function1", "MyClass", "my_func", function(self, arg, arg2) end)
> Modify.createHook("function2", "MyClass", "my_func__2", function(self, arg, arg2, extraArg) end) -- This is a function that overloads `my_func`. Internally, it holds the same name but different parameters.
> -- Do the same for other overloads (e.g. `my_func__3`, `my_func__4`, etc.)
> ```
### Unsupported function parameters
- Modify omits certain functions due to a technical limitation by the Sol2 Lua API that Modify relies on heavily.
- Functions that take non-const references (`T&`) and functions that take `std::unordered_map`.
- Example:
> ```c++
> std::string& hello2; // Will be omitted.
> std::unordered_map<T1, T2> helloArray; // Will be omitted.
>
> std::string hello3; // Will not be omitted.
> const std::string& hello; // Will not be omitted
> ```
- If your function has a Windows address for a function, check its parameters.


## Possible Question:
- Q: Why does Geode show in the console that a hook was applied even though the script doesn't apply it?
- A: The way Modify works internally, it uses one hook for every function that gets hooked. And simply calls all the functions that hook it. This is in order to preserve the original call chain. So that the original function will not get called twice because of two scripts that call it twice. All the applyHook function do is simply add the function to the call chain of the list of detours.
- Q: The function I want to hook is in `lindings.bro`, why can't I hook it?
- A: Only functions that have a Windows address can be hooked. `win inline`, the absence of a `win 0xAddress`, and the return of the function being `TodoReturn` mean that the function cannot be hooked as of now. If it does have an address, Modify omits certain functions due to technical limitation, view the [Special Behavior](#special-behavior) section in this README.