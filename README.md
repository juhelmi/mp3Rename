# mp3Rename
Feature study

Contains cmake, boots, json and other studies. This could be done faster with Python :)

## Git submodules

Used submodules are listed in .gitmodules. Contains path and url.
Add submodules with following commands

```
    cd external
    git submodule add git@github.com:taglib/taglib.git
    git submodule add git@github.com:nlohmann/json.git
```

After that
```
    cd ..
    make all
```

