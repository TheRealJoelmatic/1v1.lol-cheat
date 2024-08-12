# THIS CHEAT NO LONGER WORKS 
Thats the reason im releasing this source code. (This is not made to be skided or pasted)


# Information 

This was a cheat made for the il2cpp version 1v1.lol.
When the game updated to mono this cheats stoped working and as IL2CPP_Resolver is made for IL2CPP.

All my methods in this cheats, I no longer use in my newer cheats. This is released purely for learning purposes in making cheats for il2cpp unity games.

This src can easily be updated to other unity games.


# Code overview

## Improvements

The first thing you will notice is all the module's are coded in a singler file [sdk.cpp](https://github.com/TheRealJoelmatic/1v1.lol-cheat/blob/main/sdk/sdk.cpp). This could be improved to an class module system an [good example of this is](https://github.com/Lefraudeur/RiptermsGhost/blob/8f1d786feca15d2eae0de7614e5965843a0de903/Ripterms/Modules/Modules.h#L25).

One notable issue in the current code is the use of threads to call the [getplayers()](https://github.com/TheRealJoelmatic/1v1.lol-cheat/blob/main/sdk/sdk.cpp#L101) function without any mutex or locking mechanism.  Creates as race condition were in some case will result in an crash.

Empty try-catch blocks, is an missed opportunity to log when somthing goes wrong and fix the core problem rather than an bandage fix.

Lastly the use of magic values this is somthing im actually trying to stop doing as it makes the code harder to read even if I commet what the value is. [An great example is the bone array](https://github.com/TheRealJoelmatic/1v1.lol-cheat/blob/main/sdk/sdk.cpp#L399), here we can see an basicly a bunch of random numbers with no meaning. A better way to do this is making an list with the name of the bone, this how i do it in my apex cheat now.

(My apex cheat src)
 ```
 enum class Bone : int {
    Head = 0,
    Neck = 1,
    UpperChest = 2,
    LowerChest = 3,
    Stomach = 4,
    Hip = 5,
    Leftshoulder = 6,
    Leftelbow = 7,
    Lefthand = 8,
    Rightshoulder = 9,
    RightelbowBone = 10,
    Righthand = 11,
    LeftThighs = 12,
    Leftknees = 13,
    Leftleg = 14,
    RightThighs = 16,
    Rightknees = 17,
    Rightleg = 18,
};
 ```

 ## What went well

First its a working 1v1.lol cheat with a soild base that can be used in alomost any other IL2CPP unity game.

Secondly the logger was my own custom one made for internal cheats witch I was proud of, It still can be improved lot.

# Whats is missing

Well ive striped certain files form this project stuch as my auth checks and the il2cpp.h

# References 

- https://github.com/AndnixSH/Il2CppDumper-GUI
- https://github.com/IdolTheDev/Il2CPP-Base
- https://www.unknowncheats.me/forum/unity/476697-il2cpp-resolver.html

# Tools used

- https://www.cheatengine.org/
- https://hex-rays.com/ida-pro/
- https://github.com/ReClassNET/ReClass.NET
- https://github.com/AndnixSH/Il2CppDumper-GUI
- https://code.visualstudio.com/
- https://visualstudio.microsoft.com/vs/