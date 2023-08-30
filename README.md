# ONEOS
**School name:** Northwestern Polytechnical University  
**Team members:** Hira Khyzer, Datago Abdirezak Mundino, Apedo Komi Etse Yvon, Rana Rashid, Satyam

## Content:
1. JavaScript component support, you can directly choose jerryScript and iotjs currently supported by OneOS (but not fully compatible with lv_binding_js), or choose quickjs (lv_binding_js' js execution engine dependency) and port it to OneOS by yourself;
2. Transplant the lv_binding_js framework in the LVGL open-source project to OneOS;
3. Output a performance comparison report with the OneOS light application framework one-evue, such as resource usage.

## OneOS Overview
- OneOS is a lightweight operating system launched by China Mobile for the Internet of Things field. It has the characteristics of tailoring, cross-platform, low power consumption, and high security. It supports mainstream chips such as ARM Cortex-A and Cortex-M, MIPS, and RISC-V. Architecture, compatible with standard interfaces such as POSIX and CMSIS, supports advanced language development modes such as Javascript and MicroPython, provides graphical development tools, can effectively improve development efficiency, reduce development costs, and help users quickly develop stable, reliable, safe and easy-to-use IoT applications;
- Currently, OneOS supports jerryScript and iotjs operating environments, and can parse and execute js code.

## lv_binding_js
- The lv_binding_js framework is an open source project based on LVGL, which supports writing LVGL using JavaScript. It uses react's virtual DOM concept to operate LVGL UI components, and developers can use front-end UI development specifications and habits for graphical interface development, providing users with a simple and fast development experience;
- The operation of lv_binding_js depends on the quickjs execution engine.

## quickjs
- quickjs is a lightweight and embeddable JavaScript engine that supports the ES2019 specification, including ES modules, asynchronous generators and proxies;
- It is suitable for embedded devices, mobile applications and other resource-constrained environments, featuring fast startup and low memory footprint. Through a simple API and extension mechanism, developers can embed and extend JavaScript functionality in applications. At present, quickjs has been widely used. For example, quickjs is used as the js execution engine in ALIOS.

## Project Construction & Development Implementation
### Environmental preparation
1. Install the compilation tool OneOS-cube [Download address](https://os.iot.10086.cn/download/tool), the installation and use of OneOS-cube can be viewed in the documentation
2. Install the serial port debugging tool MobaXterm
3. Clone the project to the local, OneOS project repository address: [OneOS Repository](https://gitee.com/cmcc-oneos/OneOS.git)

### Build Project
1. Enter the directory `.\OneOS\projects` (this directory is the path where the OneOS source code is located), right click to open OneOS-cube
2. Execute the following command to create a new project: `oos project`
3. Select the development board model that needs to build the project, we choose STM32F103ZE.

### Connect the Development Board
The model of the development board we use is Wanou Tiangong STM32F103ZE, turn on the power supply of the development board, and connect the host computer to the development board through the STM32 downloader for subsequent programming.

### Build
1. Enter the `.\OneOS\projects\STM32F103ZE` directory, right-click to open OneOS-cube, and execute the following command to configure OneOS and enable corresponding functional components: `oos config`
2. After completing the configuration, save and exit, execute the following command to start the build: `oos build`
3. OneOS-cube supports converting the project into a keil project, compiling and burning directly in keil, executing the following command, or executing the `oos build` direct compilation: `oos init -i keil`
4. After OneOS-cube executes the build successfully, it will generate corresponding files in the `.\OneOS\projects\STM32F103ZE\out` directory.

### Development and Implementation
1. Virtual machine compilation test lv_binding_js
    - In order to further familiarize yourself with and understand the functions and design logic of lv_binding_js, we first compiled it on a virtual machine and tested the provided demo to prepare for the next step of transplanting lv_binding_js.
    - lv_binding_js provides guidance on how to build on Ubuntu, follow the prompts to complete the required environment and tool installation.
    - Note: According to our test, in addition to the tools in the prompt, node needs to be installed to compile lv_binding_js correctly.
    - Download the source code locally. It should be noted that since the lv_binding_js repository refers to other repositories as submodules, it is necessary to clone all dependent submodule repositories, otherwise, errors of missing dependencies will occur during compilation and cannot be compiled correctly.

2. Transplant quickjs
    - The OneOS project uses components to develop the functions that the operating system needs to support, and the relevant code needs to be moved to the `/OneOS/components/` path for compilation and use.
    - The OneOS project uses the Kconfig file to configure the properties of the components that need to be compiled, and provides a visual operation interface in OneOS-cube; in addition, the weave.yaml file is used to configure the source files that need to be compiled, their header file reference paths, and macro dependencies.

### Source Code Preparation
1. Download the quickjs source code, you can go to the quickjs open source repository to clone
2. Move the quickjs folder to the `\\OneOS\\components\\` path in the OneOS project.

### Compile Configuration
1. Enter OneOS-cube, open the quickjs component and add to compile
2. Enter the `\\OneOS\\components\\` path, open the weave.yaml file, and add quickjs configuration options


## Compile and Debug

1. Modify the gcc compilation configuration, open the `settings-gcc.yaml` file under the path of the project file (`\\OneOS\\projects\\STM32F103ZE`), and replace the corresponding code with the following content:
```
- ' -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -ffunction-sections fdata-sections -D__ONEOS__ -Wno-array-bounds -Wno-format-truncation -D_GNU_SOURCE DCONFIG_VERSION=\\\"2021-03-27\\\" -DCONFIG_BIGNUM'
```

2. Modify the header file reference part of the `qjsc.c` file under the path `\\OneOS\\components\\quickjs\\quickjs.c` as follows:
```c
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#ifdef __ONEOS__
#include "oneos_amp_port.h"
#else
#include "fenv.h"
#include "linux_jquick_mutex.h"
#endif // __ONEOS__
#if defined(_WIN32)
#include <windows.h>
#include <malloc.h>
#endif
#include <math.h>
#if defined(__APPLE__)
#include <malloc/malloc.h>
#elif defined(__linux__)
#include <malloc.h>
#endif
#include "cutils.h"
#include "list.h"
#include "quickjs.h"
#include "libregexp.h"
#ifdef CONFIG_BIGNUM
#include "libbf.h"
#endif
```

3. Add shell execution entry function `qjs.c`, Add the following code at the end of the file to provide a shell function execution entry to ensure that it can be executed through shell script commands:
```c
SH_CMD_EXPORT(quickjs, quickjs_main, "Show example quickjs");
```

At the same `qjs.c` time, add the header file reference to ensure that the compilation passes:
```c
#include <board.h>
#include <os_memory.h>
#include <shell.h>
```

### Transplant lv_binding_js

- As mentioned above, `lv_binding_js` supports the use of JavaScript to write LVGL, and uses react's virtual DOM concept to operate LVGL UI components, realizing the development of visual applications according to the habits of front-end programming languages.
- The source code structure of `lv_binding_js` is shown in the figure below (Note: The figure is not provided in this markdown format).

#### Source code preparation

1. Execute the following command to download the source code of the `lv_binding_js` framework to the local:
```
git clone https://github.com/lvgl/lv_binding_js --recurse-submodules
```

Since the `lv_binding_js` repository refers to other repositories as submodules, it is necessary to clone all dependent submodule repositories, otherwise there will be errors of missing dependencies during the compilation process, and it cannot be compiled correctly. If an error occurs in the above command or the code cannot be cloned normally, the following code can be executed:
```
git clone https://gitee.com/the_trees_are_shallow/lv_binding_js.git --recurse-submodules
```

2. Same as transplanting `quickjs` in the second stage, add the `lv_binding_js` code `/OneOs/components/` to the path to start and use it as a component; then write the configuration file to add `lv_binding_js` to the compilation, and `/OneOs/components/weave.yaml` add it in the last line.
```
-lv_binding_js
```

Then at `/OneOS/components/lv_binding_js` create a new `weave.yaml` file under the path and add the following code:
```
group_name: lv_binding_js
depend_macro:
- USING_lv_binding_js
add_subdirectory:
- src/deps/libtuv
- src/deps/lvgl8.2/yaml
- src/deps/lv_drivers
- src/jsruntime
- src/utils
- src/engine
- src/render
```

Then at `/OneOS/components/lv_binding_js` create a new file under the path `Kconfig` and add the following code:
```
menu "lv_binding_js"
config USING_lv_binding_js
bool "enable lv_binding_js"
select USING_LVGL_8.2
default n
menuconfig USING_LVGL_8.2
bool "Enable LVGL8.2"
default n
```

3. `lv_binding_js` compilation depends on other components and libraries, as shown in the source code structure diagram, in order to facilitate unified management and compilation, we moved the `lvgl`, `libtuv` and `quickjs` modules (which are already supported by OneOS) into the corresponding positions in the `lv_binding_js` directory to ensure that the compilation is passed.

#### Compile and debug

Because the code structure in `lv_binding_js` is relatively large, we adopted the idea of gradual compilation during the transplantation process, divided the overall code file into several independent and irrelevant parts, and then compiled and debugged sequentially through the `weave.yaml` file configuration.

1. Compile and debug `/src/utils`
   - Create a new configuration file `weave.yaml`, add compiled files and header file reference paths
   - Comment `./utils/util.c` files `readlink()` and `dirname()` function calls
   - Make sure the command `oos build` compiles



2. Compile and debug /src/jsruntime

- In addition to quickjs support, lv_binding_js also needs to compile *.c files in the current path.
- Create a new configuration file weave.yaml, add the compilation file and header file reference path, please refer to the corresponding file of this warehouse for details.
- Due to the interdependence between some of the files, they need to be added at the same time for compilation. For details, you can use the compilation error message to view.

Note that due to the need to rely on components\\lv_binding_js\\src\\deps\\libtuv\\include\\uv.h files here, some functions are called directly without declarations, resulting in errors in the compilation process. The reason is that there are no related function declarations and implementations in the current libtuv, so it is necessary to complete the missing function declarations and implementations in the libuv corresponding path uv.h, uv-common.c, and files; at the same time, ensure that they are called correctly uv-common.hfs.c.

3. Transplant lv_binding_js

- As mentioned above, lv_binding_js supports the use of JavaScript to write LVGL, and uses react's virtual DOM concept to operate LVGL UI components, realizing the development of visual applications according to the habits of front-end programming languages.
- The source code structure of lv_binding_js is shown in the figure below (figure not included).

### Source code preparation

1. Execute the following command to download the source code of the lv_binding_js framework to the local:
```
git clone https://github.com/lvgl/lv_binding_js --recurse-submodules
```

Since the lv_binding_js repository refers to other repositories as submodules, it is necessary to clone all dependent submodule repositories, otherwise there will be errors of missing dependencies during the compilation process, and it cannot be compiled correctly. If an error occurs in the above command or the code cannot be cloned normally, the following code can be executed:
```
git clone https://gitee.com/the_trees_are_shallow/lv_binding_js.git --recurse-submodules
```

2. Same as transplanting quickjs in the second stage, add the lv_binding_js code /OneOs/components/ to the path to start and use it as a component; then write the configuration file to add lv_binding_js to the compilation, and /OneOs/components/weave.yaml add it in the last line.
```
-lv_binding_js
```

Then at /OneOS/components/lv_binding_js create a new weave.yaml file under the path and add the following code:
```
group_name: lv_binding_js
depend_macro:
- USING_lv_binding_js
add_subdirectory:
- src/deps/libtuv
- src/deps/lvgl8.2/yaml
- src/deps/lv_drivers
- src/jsruntime
- src/utils
- src/engine
- src/render
```

Then at /OneOS/components/lv_binding_js create a new file under the path Kconfig and add the following code:
```
menu "lv_binding_js"
config USING_lv_binding_js
bool "enable lv_binding_js"
select USING_LVGL_8.2
default n
menuconfig USING_LVGL_8.2
bool "Enable LVGL8.2"
default n
```

3. lv_binding_js compilation depends on other components and libraries, as shown in the source code structure diagram, in order to facilitate unified management and compilation, we moved the lvgl , libtuv and quickjs modules (which are already supported by OneOS) into the corresponding positions in the lv_binding_js directory to ensure that the compilation is passed.

### Compile and debug

Because the code structure in lv_binding_js is relatively large, we adopted the idea of gradual compilation during the transplantation process, divided the overall code file into several independent and irrelevant parts, and then compiled and debugged sequentially through the weave.yaml file configuration.

1. Compile and debug /src/utils
   - Create a new configuration file weave.yaml, add compiled files and header file reference paths.
   - Comment ./utils/util.cfiles readlink() and dirname()function calls.
   - Make sure the command oos build compiles.

2. Compile and debug /src/jsruntime (as detailed above)

3. The next part contains the .cpp files. In order to compile them, you need to configure the g++ compilation tool. Execute the command oos config to enter the compilation configuration selection interface, open the c++ compilation support option, and save and exit.

4. Compile and debug /src/engine
   - Create a new configuration file weave.yaml, add compiled files and header file reference paths.
   - Make sure the command oos build compiles through.

5. Compile and debug /src/render
   - Create a new configuration file weave.yaml, add the compilation file and header file reference path.
   - The case of some paths in the source file is not uniform and needs to be corrected.
   - Make sure the command oos build compiles through.

### js code transplantation

- As a scripting language, js code can be used without compilation, and quickjs already supports the execution of js language.
- After the compilation link is passed, write render/react(copy) the folder to the development board through the SD card for subsequent testing.
- Add the following code at engine.cpp at the end of the file to provide the shell function execution entry to ensure that it can be executed through the shell script command.



## Difficulties and Solutions

### Difficulty

- **Environment Incompatibility**: Since the whole migration process involves many frameworks, libraries, compilation tools, and operating systems, many environment incompatible errors (500+) will appear during the compilation process, mainly in undeclared variables and functions, function parameters, and return value types. These errors include variables not being uniform, reference header file absence, and more.
- **RAM Overflow**: After the entire code is compiled, it is found that there are RAM overflows, resulting in a link error.

### Solution

- **For Compilation Errors Caused by Environment Incompatibility**:
  - If the relevant variables and functions are not further called and executed, you can choose to temporarily comment out the error location.
  - Alternatively, you can choose to declare and initialize the variables yourself.
  - If it is a library function, you can check if this function exists on both Windows and Linux platforms.
- **For the RAM Overflow Problem**:
  - Choose to add external expansion RAM and flash to increase the RAM of the development board and solve the overflow problem.

## Result Display & Result Analysis

You can directly use the compiled and linked bin file in this repository `\OneOS\projects\STM32F103ZE\out` to burn it into the STM32F103ZE development board. Alternatively, you can compile and link it yourself to generate bin and ELF documents.

### Build

1. After the configuration and debugging are completed, navigate to the `\OneOS\projects\STM32F103ZE` path, right-click to open OneOS Cube, and execute the following command to clear the current compiled content:
   
   ```
   oos clean
   ```
   
2. Execute the following command and wait for the compilation to complete:
   
   ```
   oos build
   ```
   
3. After the compilation (build) and (link) are completed, the `out` folder and the corresponding files inside will be generated in the project file for burning.

### Test QuickJS

1. Burn the compiled bin file to the development board.
2. Open the serial port debugging tool MobaXterm for code debugging.
3. Open the shell terminal under OneOS, and you can see the script commands that can be used after the current configuration is completed, including the QuickJS we just compiled.
4. Create a new JS file, execute the QuickJS script command, and test the execution of QuickJS.


# Summary 


### OneOS Overview

- OneOS is a lightweight operating system developed by China Mobile for IoT applications. It's tailored, cross-platform, energy-efficient, and secure.
- It supports chips like ARM Cortex-A, Cortex-M, MIPS, and RISC-V, and is compatible with interfaces like POSIX and CMSIS.
- OneOS supports jerryScript and iotjs for JavaScript execution.

### lv_binding_js

- An open-source project based on LVGL that allows LVGL operations using JavaScript.
- It uses React's virtual DOM for LVGL UI components, offering a fast development experience.
- It depends on the quickjs execution engine.

### quickjs

- A lightweight JavaScript engine supporting the ES2019 specification.
- Suitable for embedded devices due to its fast startup and low memory footprint.

### Project Construction & Development Implementation

- The report provides a step-by-step guide on setting up the environment, cloning the project, building the project, connecting the development board, and other related tasks.

### Transplantation of quickjs and lv_binding_js

- The report details the process of transplanting quickjs and lv_binding_js to OneOS. This includes preparing the source code, configuring the compilation, and debugging.

### Difficulties and Solutions

- Challenges faced during the transplantation process include environment incompatibility and RAM overflow.
- Solutions include commenting out unnecessary code, declaring and initializing variables, and adding external RAM.

### Result Display & Analysis

- The report provides instructions on how to build the project, test quickjs, and execute scripts on the development board.

---

