#!/usr/bin/env bash

# exit on any error
set -Ee

# Enter the directory that contains this script file
cd $(dirname $0)
TOP=$( pwd -L )

if [ $(id -u) -eq 0 ] ; then
    echo "This script should not be run as root or with sudo."
    return 1
fi

# function to display menus
show_menus() {
    clear
    echo "                     "
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " WELCOME TO LIBQMYCROFT INSTALLATION SCRIPT "
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo "                     "
    echo "Please Note: This is an Interactive script that will take you through a series of installation choices, where you might be required to provide your administrative password to successfully install system dependencies and Libqmycroft on your system."
    echo "                     "
    echo "~~~~~~~~~~~~~~~~~~~~~"
    echo " SELECT - DISTRIBUTION "
    echo "~~~~~~~~~~~~~~~~~~~~~"
    echo "1. KDE NEON BIONIC"
    echo "2. K/UBUNTU 20.04+ / KDE NEON FOCAL"
    echo "3. MANJARO/ARCH"
    echo "4. ALPINE LINUX"
    echo "5. openSUSE Leap 15.x"
    echo "6. OTHERS"
    echo "7. UPDATE INSTALLATION"
    echo "8. EXIT"
}

read_options() {
    echo " "
	local choice
	read -p "Enter choice [ 1 - 8 ] " choice
	case $choice in
		1) neon ;;
		2) kubuntu ;;
		3) manjaro ;;
		4) alpine ;;
		5) opensuse ;;
		6) others ;;
		7) updateinstall;;
		8) exit 0;;
		*) echo -e "${RED}Error...${STD}" && sleep 2
	esac
}

#trap '' SIGINT SIGQUIT SIGTSTP

function found_exe() {
   hash "$1" 2>/dev/null
}

neon() {
    echo "Starting Installation For KDE NEON"
    echo ""
    echo "Following Packages Will Be Installed: git-core g++ cmake extra-cmake-modules gettext pkg-config pkg-kde-tools qtbase5-dev qtdeclarative5-dev libqt5websockets5-dev"
    echo ""
    echo "Please Enter Authentication For Installing System Dependencies"
    sudo apt-get install -y git-core g++ cmake extra-cmake-modules gettext pkg-config pkg-kde-tools qtbase5-dev qtdeclarative5-dev libqt5websockets5-dev
    build_lib
}

kubuntu() {
    echo "Starting Installation For K/Ubuntu 20.04 +"
    echo ""
    echo "Following Packages Will Be Installed: git-core g++ cmake extra-cmake-modules gettext pkg-config pkg-kde-tools qtbase5-dev qtdeclarative5-dev libqt5websockets5-dev"
    echo ""
    echo "Please Enter Authentication For Installing System Dependencies"
    sudo apt-get install -y git-core g++ cmake extra-cmake-modules gettext pkg-config pkg-kde-tools qtbase5-dev qtdeclarative5-dev libqt5websockets5-dev
    build_lib
}

manjaro() {
    echo "Starting Installation For Manjaro / Arch"
    echo ""
    echo "Following Packages Will Be Installed: cmake extra-cmake-modules qt5-websockets qt5-declarative qt5-quickcontrols2 qt5-base"
    echo ""
    echo "Please Enter Authentication For Installing System Dependencies"
    yes | sudo pacman -S git cmake extra-cmake-modules qt5-websockets qt5-declarative qt5-quickcontrols2 qt5-base
    build_lib   
}

alpine() {
    echo "Starting Installation For Alpine Linux"
    echo ""
    echo "The development headers for the following packages will be installed: alpine-sdk cmake extra-cmake-modules qt5-qtwebsockets qt5-qtdeclarative qt5-qtquickcontrols2 qt5-qtbase"
    echo "They can easily be uninstalled later on by running 'apk del makedeps-libqmycroft'"
    echo ""
    echo "Please Enter Authentication For Installing System Dependencies"
    sudo apk add --virtual makedeps-libqmycroft alpine-sdk cmake extra-cmake-modules qt5-qtwebsockets-dev qt5-qtdeclarative-dev qt5-qtquickcontrols2-dev qt5-qtbase-dev
    build_lib
}

opensuse() {
    echo "Starting Installation For openSUSE Leap 15.x"
    echo ""
    echo "The development headers for the following packages will be installed: cmake extra-cmake-modules libqt5-qtwebsockets libqt5-qtdeclarative libQt5QuickControls2 libqt5-qtbase"
    echo ""
    echo "Please Enter Authentication For Installing System Dependencies"
    sudo zypper --non-interactive install cmake extra-cmake-modules libqt5-qtwebsockets-devel libqt5-qtwebview-devel libqt5-qtdeclarative-devel libQt5QuickControls2-devel libqt5-qtbase-devel
    build_lib
}

updateinstall() {
    echo "Pulling Latest Changes From Master"
    git pull origin master
    echo "Update Completed"
    exit 0
    build_lib
}

continue_unknown() {
    echo "Starting Installation For Unknown Platform, Builds Will Fail If Required Packages Are Not Found"
    build_lib
}

return_previous() {
    show_menus
}

others() {
      clear
      echo "You must manually install the following packages for this platform"
      echo "cmake extra-cmake-modules qt5-websockets qt5-declarative qt5-quickcontrols2 qt5-base"
      echo "Consider contributing support for your platform by adding it to this script"
      
      echo "1. Continue Installation"
      echo "2. Return To Previous Menu"
      echo "3. Exit"
      
      local additional_choice
      read -p "Enter choice [ 1 - 3 ] " additional_choice
      case $additional_choice in
          1) continue_unknown;;
          2) return_previous;;
          3) exit 0;;
      esac
}

function build_lib() {
    echo " "
    echo "Building Libqmycroft"
    if [[ ! -d build-testing ]] ; then
    mkdir build-testing
    fi
    cd build-testing
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release   -DKDE_INSTALL_LIBDIR=lib -DKDE_INSTALL_USE_QT_SYS_PATHS=ON
    make -j4
    sudo make install
    install_libqmycroft_mock
}

function install_libqmycroft_mock() {
    echo " "
    echo "Installing Libqmycroft Mock Interface"
    cd ..
    mkdir -p /opt/mycroft/skills/libqmycroft-mock-interface 
    cp -R libqmycroft-mock-interface/* /opt/mycroft/skills/libqmycroft-mock-interface
    complete_installer
}   

function complete_installer() {
    echo " "
    echo "Installation complete!"
    exit 0
}

while true
do

	show_menus
	read_options
done
