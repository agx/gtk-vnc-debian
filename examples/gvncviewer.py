#!/usr/bin/python

# GTK VNC Widget
#
# Copyright (C) 2006  Anthony Liguori <anthony@codemonkey.ws>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.0 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA

from gi.repository import Gtk
from gi.repository import Gdk
from gi.repository import GtkVnc
import sys

if len(sys.argv) != 2 and len(sys.argv) != 3:
    print "syntax: gvncviewer.py host:display [password]"
    sys.exit(1)

def set_title(vnc, window, grabbed):
    name = vnc.get_name()
    keys = vnc.get_grab_keys()
    keystr = None
    for i in range(keys.nkeysyms):
        k = keys.get_nth(i)
        if keystr is None:
            keystr = Gdk.keyval_name(k)
        else:
            keystr = keystr + "+" + Gdk.keyval_name(k)
    if grabbed:
        subtitle = "(Press %s to release pointer) " % keystr
    else:
        subtitle = ""

    window.set_title("%s%s - GVncViewer" % (subtitle, name))

def vnc_screenshot(src, ev, vnc):
    if ev.keyval == Gdk.keyval_from_name("F11"):
        pix = vnc.get_pixbuf()
        pix.save("gvncviewer.png", "png", { "tEXt::Generator App": "gvncviewer.py" })
        print "Screenshot saved to gvncviewer.png"

    return False

def vnc_grab(src, window):
    set_title(src, window, True)

def vnc_ungrab(src, window):
    set_title(src, window, False)

def vnc_connected(src):
    print "Connected to server"

def vnc_initialized(src, window):
    print "Connection initialized"
    set_title(src, window, False)
    window.show_all()

def vnc_disconnected(src):
    print "Disconnected from server"
    Gtk.main_quit()

def send_caf1(src, vnc):
    print "Send Ctrl+Alt+F1"
    vnc.send_keys(["Control_L", "Alt_L", "F1"])

def send_caf7(src, vnc):
    print "Send Ctrl+Alt+F7"
    vnc.send_keys(["Control_L", "Alt_L", "F7"])

def send_cad(src, vnc):
    print "Send Ctrl+Alt+Del"
    vnc.send_keys(["Control_L", "Alt_L", "Del"])

def send_cab(src, vnc):
    print "Send Ctrl+Alt+BackSpace"
    vnc.send_keys(["Control_L", "Alt_L", "BackSpace"])

def vnc_auth_cred(src, credList):
    prompt = 0
    data = []

    print type(credList)
    for i in range(credList.n_values):
        data.append(None)
        if credList.get_nth(i) in (GtkVnc.DisplayCredential.USERNAME, GtkVnc.DisplayCredential.PASSWORD):
            prompt = prompt + 1
        elif credList.get_nth(i) == GtkVnc.DisplayCredential.CLIENTNAME:
            data[i] = "gvncviewer"

    if prompt:
        dialog = Gtk.Dialog("Authentication required", None, 0, (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL, Gtk.STOCK_OK, Gtk.ResponseType.OK))
        dialog.set_default_response(Gtk.ResponseType.OK)
        label = []
        entry = []

        box = Gtk.Table(2, prompt)

        row = 0
        for i in range(credList.n_values):
            entry.append(Gtk.Entry())
            if credList.get_nth(i) == GtkVnc.DisplayCredential.USERNAME:
                label.append(Gtk.Label("Username:"))
            elif credList.get_nth(i) == GtkVnc.DisplayCredential.PASSWORD:
                label.append(Gtk.Label("Password:"))
                entry[-1].set_visibility(False)
                entry[-1].set_activates_default(True)
            else:
                entry[-1].destroy()
                continue

            box.attach(label[row], 0, 1, row, row+1, 0, 0, 3, 3)
            box.attach(entry[row], 1, 2, row, row+1, 0, 0, 3, 3)
            row = row + 1

        vbox = dialog.get_child()
        vbox.add(box)

        dialog.show_all()
        res = dialog.run()
        dialog.hide()

        if res == Gtk.ResponseType.OK:
            row = 0
            for i in range(credList.n_values):
                if credList.get_nth(i) in (GtkVnc.DisplayCredential.USERNAME, GtkVnc.DisplayCredential.PASSWORD):
                    data[i] = entry[row].get_text()
                    row = row + 1

        dialog.destroy()

    for i in range(credList.n_values):
        if i < len(data) and data[i] != None:
            if src.set_credential(credList.get_nth(i), data[i]):
                print "Cannot set credential type %d" % (credList.get_nth(i))
                src.close()
        else:
            print "Unsupported credential type %d" % (credList.get_nth(i))
            src.close()

window = Gtk.Window()
vnc = GtkVnc.Display()

layout = Gtk.VBox()
window.add(layout)

menubar = Gtk.MenuBar()
sendkeys = Gtk.MenuItem("_Send keys")
menubar.append(sendkeys)

buttons = Gtk.HBox()
caf1 = Gtk.MenuItem("Ctrl+Alt+F_1")
caf7 = Gtk.MenuItem("Ctrl+Alt+F_7")
cad = Gtk.MenuItem("Ctrl+Alt+_Del")
cab = Gtk.MenuItem("Ctrl+Alt+_Backspace")

submenu = Gtk.Menu()
submenu.append(caf1)
submenu.append(caf7)
submenu.append(cad)
submenu.append(cab)
sendkeys.set_submenu(submenu)

caf1.connect("activate", send_caf1, vnc)
caf7.connect("activate", send_caf7, vnc)
cad.connect("activate", send_cad, vnc)
cab.connect("activate", send_cab, vnc)


layout.add(menubar)
layout.add(vnc)

vnc.realize()
vnc.set_pointer_grab(True)
vnc.set_keyboard_grab(True)

# Example to change grab key combination to Ctrl+Alt+g
grab_keys = GtkVnc.GrabSequence.new([ Gdk.KEY_Control_L, Gdk.KEY_Alt_L, Gdk.KEY_g ])
vnc.set_grab_keys(grab_keys)

#v.set_pointer_local(True)

if len(sys.argv) == 3:
    vnc.set_credential(GtkVnc.DisplayCredential.PASSWORD, sys.argv[2])

disp = sys.argv[1].find(":")
if disp != -1:
    host = sys.argv[1][:disp]
    port = str(5900 + int(sys.argv[1][disp+1:]))
else:
    host = sys.argv[1]
    port = "5900"
print "Connecting to %s %s" % (host, port)

vnc.open_host(host, port)
vnc.connect("vnc-pointer-grab", vnc_grab, window)
vnc.connect("vnc-pointer-ungrab", vnc_ungrab, window)

vnc.connect("vnc-connected", vnc_connected)
vnc.connect("vnc-initialized", vnc_initialized, window)
vnc.connect("vnc-disconnected", vnc_disconnected)
vnc.connect("vnc-auth-credential", vnc_auth_cred)

window.connect("key-press-event", vnc_screenshot, vnc)

Gtk.main()
