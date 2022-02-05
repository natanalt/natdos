"use strict";

document.addEventListener("DOMContentLoaded", function() {
    let emulator = new V86Starter({
        screen_container: document.getElementById("v86-screen"),
        bios: {
            url: "seabios.bin",
        },
        vga_bios: {
            url: "vgabios.bin",
        },
        fda: {
            url: "natdos.flp",
        },
        autostart: false,
    });

    document.getElementById("start-vm-button").addEventListener("click", function() {
        emulator.run();
    });
})
