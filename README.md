# bld_test_Nucleo_STM32F401
Build of a stm32 test environment runnning wiht FREERTOS based on the STM32HAL. Cmake is used for building

1. Installation of the Toolchain:
https://launchpad.net/~team-gcc-arm-embedded/+archive/ubuntu/ppa

Step1: Inside Ubuntu, open a terminal and input
       "sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa"

Step2: Continue to input
       "sudo apt-get update"

Step3: Continue to input to install toolchain
       "sudo apt-get install gcc-arm-embedded"
       
2. Installation of cmake
 "sudo apt-get install cmake"
       
3. Clone of the repository 
  Clone of the repository and init of the corresponding git submodules
  "git submodule update --init"
   
