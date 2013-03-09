#!/usr/bin/perl

use Gtk3 -init;
Glib::Object::Introspection->setup(basename => 'GtkVnc', version => '2.0', package => 'GtkVnc');
Glib::Object::Introspection->setup(basename => 'GVnc', version => '1.0', package => 'GVnc');

GVnc::util_set_debug(1);

my $win = Gtk3::Window->new ('toplevel');
my $dpy = GtkVnc::Display->new();

$win->set_title("GTK-VNC with Perl");
$win->add($dpy);
$dpy->open_host("localhost", "5900");
$win->show_all;
Gtk3::main;
