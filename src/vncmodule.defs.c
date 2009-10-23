/* -- THIS FILE IS GENERATED - DO NOT EDIT *//* -*- Mode: C; c-basic-offset: 4 -*- */

#include <Python.h>



#line 3 "./vnc.override"
#include <Python.h>
#include "pygobject.h"
#include "vncdisplay.h"
#include "enums.h"
#line 13 "vnc.c"


/* ---------- types from other modules ---------- */
static PyTypeObject *_PyGtkDrawingArea_Type;
#define PyGtkDrawingArea_Type (*_PyGtkDrawingArea_Type)


/* ---------- forward type declarations ---------- */
PyTypeObject G_GNUC_INTERNAL PyVncDisplay_Type;

#line 24 "vnc.c"



/* ----------- VncDisplay ----------- */

static int
_wrap_vnc_display_new(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char* kwlist[] = { NULL };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,
                                     ":gtkvnc.Display.__init__",
                                     kwlist))
        return -1;

    pygobject_constructv(self, 0, NULL);
    if (!self->obj) {
        PyErr_SetString(
            PyExc_RuntimeError, 
            "could not create gtkvnc.Display object");
        return -1;
    }
    return 0;
}

static PyObject *
_wrap_vnc_display_open_fd(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "fd", NULL };
    int fd, ret;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"i:VncDisplay.open_fd", kwlist, &fd))
        return NULL;
    
    ret = vnc_display_open_fd(VNC_DISPLAY(self->obj), fd);
    
    return PyBool_FromLong(ret);

}

static PyObject *
_wrap_vnc_display_open_host(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "host", "port", NULL };
    char *host, *port;
    int ret;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"ss:VncDisplay.open_host", kwlist, &host, &port))
        return NULL;
    
    ret = vnc_display_open_host(VNC_DISPLAY(self->obj), host, port);
    
    return PyBool_FromLong(ret);

}

static PyObject *
_wrap_vnc_display_is_open(PyGObject *self)
{
    int ret;

    
    ret = vnc_display_is_open(VNC_DISPLAY(self->obj));
    
    return PyBool_FromLong(ret);

}

static PyObject *
_wrap_vnc_display_close(PyGObject *self)
{
    
    vnc_display_close(VNC_DISPLAY(self->obj));
    
    Py_INCREF(Py_None);
    return Py_None;
}

#line 17 "./vnc.override"
static PyObject*
_wrap_vnc_display_send_keys(PyGObject *self,
                            PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = {"keys", "kind", NULL};
    PyObject *keyList;
    int kind = VNC_DISPLAY_KEY_EVENT_CLICK;
    int ret, i, len;
    guint *keys;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,
                                     "O|I:VncDisplay.send_keys", kwlist,
                                     &keyList, &kind))
	return NULL;

    if (!PyList_Check(keyList))
	return NULL;

    len = PyList_Size(keyList);
    keys = malloc(sizeof(guint)*len);
    for (i = 0 ; i < len ; i++) {
        PyObject *val;
        char *sym;
        val = PyList_GetItem(keyList, i);
        sym = PyString_AsString(val);
        if (!sym) {
            free(keys);
	    return NULL;
        }
        keys[i] = gdk_keyval_from_name(sym);
    }

    vnc_display_send_keys_ex(VNC_DISPLAY(self->obj), keys, len, kind);
    free(keys);

    Py_INCREF(Py_None);
    return Py_None;
}
#line 142 "vnc.c"


static PyObject *
_wrap_vnc_display_send_pointer(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "x", "y", "button_mask", NULL };
    int x, y, button_mask;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"iii:VncDisplay.send_pointer", kwlist, &x, &y, &button_mask))
        return NULL;
    
    vnc_display_send_pointer(VNC_DISPLAY(self->obj), x, y, button_mask);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_vnc_display_set_credential(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "type", "data", NULL };
    int type, ret;
    char *data;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"is:VncDisplay.set_credential", kwlist, &type, &data))
        return NULL;
    
    ret = vnc_display_set_credential(VNC_DISPLAY(self->obj), type, data);
    
    return PyBool_FromLong(ret);

}

static PyObject *
_wrap_vnc_display_set_pointer_local(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "enable", NULL };
    int enable;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"i:VncDisplay.set_pointer_local", kwlist, &enable))
        return NULL;
    
    vnc_display_set_pointer_local(VNC_DISPLAY(self->obj), enable);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_vnc_display_get_pointer_local(PyGObject *self)
{
    int ret;

    
    ret = vnc_display_get_pointer_local(VNC_DISPLAY(self->obj));
    
    return PyBool_FromLong(ret);

}

static PyObject *
_wrap_vnc_display_set_pointer_grab(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "enable", NULL };
    int enable;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"i:VncDisplay.set_pointer_grab", kwlist, &enable))
        return NULL;
    
    vnc_display_set_pointer_grab(VNC_DISPLAY(self->obj), enable);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_vnc_display_get_pointer_grab(PyGObject *self)
{
    int ret;

    
    ret = vnc_display_get_pointer_grab(VNC_DISPLAY(self->obj));
    
    return PyBool_FromLong(ret);

}

static PyObject *
_wrap_vnc_display_set_keyboard_grab(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "enable", NULL };
    int enable;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"i:VncDisplay.set_keyboard_grab", kwlist, &enable))
        return NULL;
    
    vnc_display_set_keyboard_grab(VNC_DISPLAY(self->obj), enable);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_vnc_display_get_keyboard_grab(PyGObject *self)
{
    int ret;

    
    ret = vnc_display_get_keyboard_grab(VNC_DISPLAY(self->obj));
    
    return PyBool_FromLong(ret);

}

static PyObject *
_wrap_vnc_display_set_read_only(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "enable", NULL };
    int enable;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"i:VncDisplay.set_read_only", kwlist, &enable))
        return NULL;
    
    vnc_display_set_read_only(VNC_DISPLAY(self->obj), enable);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_vnc_display_get_read_only(PyGObject *self)
{
    int ret;

    
    ret = vnc_display_get_read_only(VNC_DISPLAY(self->obj));
    
    return PyBool_FromLong(ret);

}

static PyObject *
_wrap_vnc_display_get_pixbuf(PyGObject *self)
{
    GdkPixbuf *ret;

    
    ret = vnc_display_get_pixbuf(VNC_DISPLAY(self->obj));
    
    /* pygobject_new handles NULL checking */
    return pygobject_new((GObject *)ret);
}

static PyObject *
_wrap_vnc_display_get_width(PyGObject *self)
{
    int ret;

    
    ret = vnc_display_get_width(VNC_DISPLAY(self->obj));
    
    return PyInt_FromLong(ret);
}

static PyObject *
_wrap_vnc_display_get_height(PyGObject *self)
{
    int ret;

    
    ret = vnc_display_get_height(VNC_DISPLAY(self->obj));
    
    return PyInt_FromLong(ret);
}

static PyObject *
_wrap_vnc_display_get_name(PyGObject *self)
{
    const gchar *ret;

    
    ret = vnc_display_get_name(VNC_DISPLAY(self->obj));
    
    if (ret)
        return PyString_FromString(ret);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_vnc_display_client_cut_text(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "text", NULL };
    char *text;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"s:VncDisplay.client_cut_text", kwlist, &text))
        return NULL;
    
    vnc_display_client_cut_text(VNC_DISPLAY(self->obj), text);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_vnc_display_set_lossy_encoding(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "enable", NULL };
    int enable;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"i:VncDisplay.set_lossy_encoding", kwlist, &enable))
        return NULL;
    
    vnc_display_set_lossy_encoding(VNC_DISPLAY(self->obj), enable);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_vnc_display_get_lossy_encoding(PyGObject *self)
{
    int ret;

    
    ret = vnc_display_get_lossy_encoding(VNC_DISPLAY(self->obj));
    
    return PyBool_FromLong(ret);

}

static PyObject *
_wrap_vnc_display_set_scaling(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "enable", NULL };
    int enable, ret;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"i:VncDisplay.set_scaling", kwlist, &enable))
        return NULL;
    
    ret = vnc_display_set_scaling(VNC_DISPLAY(self->obj), enable);
    
    return PyBool_FromLong(ret);

}

static PyObject *
_wrap_vnc_display_get_scaling(PyGObject *self)
{
    int ret;

    
    ret = vnc_display_get_scaling(VNC_DISPLAY(self->obj));
    
    return PyBool_FromLong(ret);

}

static PyObject *
_wrap_vnc_display_set_force_size(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "enable", NULL };
    int enable;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"i:VncDisplay.set_force_size", kwlist, &enable))
        return NULL;
    
    vnc_display_set_force_size(VNC_DISPLAY(self->obj), enable);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_vnc_display_get_force_size(PyGObject *self)
{
    int ret;

    
    ret = vnc_display_get_force_size(VNC_DISPLAY(self->obj));
    
    return PyBool_FromLong(ret);

}

static PyObject *
_wrap_vnc_display_set_shared_flag(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "shared", NULL };
    int shared;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"i:VncDisplay.set_shared_flag", kwlist, &shared))
        return NULL;
    
    vnc_display_set_shared_flag(VNC_DISPLAY(self->obj), shared);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_vnc_display_get_shared_flag(PyGObject *self)
{
    int ret;

    
    ret = vnc_display_get_shared_flag(VNC_DISPLAY(self->obj));
    
    return PyBool_FromLong(ret);

}

static PyObject *
_wrap_vnc_display_set_depth(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "depth", NULL };
    PyObject *py_depth = NULL;
    VncDisplayDepthColor depth;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"O:VncDisplay.set_depth", kwlist, &py_depth))
        return NULL;
    if (pyg_enum_get_value(VNC_TYPE_DISPLAY_DEPTH_COLOR, py_depth, (gpointer)&depth))
        return NULL;
    
    vnc_display_set_depth(VNC_DISPLAY(self->obj), depth);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_vnc_display_get_depth(PyGObject *self)
{
    gint ret;

    
    ret = vnc_display_get_depth(VNC_DISPLAY(self->obj));
    
    return pyg_enum_from_gtype(VNC_TYPE_DISPLAY_DEPTH_COLOR, ret);
}

static PyObject *
_wrap_vnc_display_force_grab(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "enable", NULL };
    int enable;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"i:VncDisplay.force_grab", kwlist, &enable))
        return NULL;
    
    vnc_display_force_grab(VNC_DISPLAY(self->obj), enable);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_vnc_display_is_pointer_absolute(PyGObject *self)
{
    int ret;

    
    ret = vnc_display_is_pointer_absolute(VNC_DISPLAY(self->obj));
    
    return PyBool_FromLong(ret);

}

static PyObject *
_wrap_vnc_display_request_update(PyGObject *self)
{
    int ret;

    
    ret = vnc_display_request_update(VNC_DISPLAY(self->obj));
    
    return PyBool_FromLong(ret);

}

static const PyMethodDef _PyVncDisplay_methods[] = {
    { "open_fd", (PyCFunction)_wrap_vnc_display_open_fd, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "open_host", (PyCFunction)_wrap_vnc_display_open_host, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "is_open", (PyCFunction)_wrap_vnc_display_is_open, METH_NOARGS,
      NULL },
    { "close", (PyCFunction)_wrap_vnc_display_close, METH_NOARGS,
      NULL },
    { "send_keys", (PyCFunction)_wrap_vnc_display_send_keys, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "send_pointer", (PyCFunction)_wrap_vnc_display_send_pointer, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "set_credential", (PyCFunction)_wrap_vnc_display_set_credential, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "set_pointer_local", (PyCFunction)_wrap_vnc_display_set_pointer_local, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "get_pointer_local", (PyCFunction)_wrap_vnc_display_get_pointer_local, METH_NOARGS,
      NULL },
    { "set_pointer_grab", (PyCFunction)_wrap_vnc_display_set_pointer_grab, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "get_pointer_grab", (PyCFunction)_wrap_vnc_display_get_pointer_grab, METH_NOARGS,
      NULL },
    { "set_keyboard_grab", (PyCFunction)_wrap_vnc_display_set_keyboard_grab, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "get_keyboard_grab", (PyCFunction)_wrap_vnc_display_get_keyboard_grab, METH_NOARGS,
      NULL },
    { "set_read_only", (PyCFunction)_wrap_vnc_display_set_read_only, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "get_read_only", (PyCFunction)_wrap_vnc_display_get_read_only, METH_NOARGS,
      NULL },
    { "get_pixbuf", (PyCFunction)_wrap_vnc_display_get_pixbuf, METH_NOARGS,
      NULL },
    { "get_width", (PyCFunction)_wrap_vnc_display_get_width, METH_NOARGS,
      NULL },
    { "get_height", (PyCFunction)_wrap_vnc_display_get_height, METH_NOARGS,
      NULL },
    { "get_name", (PyCFunction)_wrap_vnc_display_get_name, METH_NOARGS,
      NULL },
    { "client_cut_text", (PyCFunction)_wrap_vnc_display_client_cut_text, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "set_lossy_encoding", (PyCFunction)_wrap_vnc_display_set_lossy_encoding, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "get_lossy_encoding", (PyCFunction)_wrap_vnc_display_get_lossy_encoding, METH_NOARGS,
      NULL },
    { "set_scaling", (PyCFunction)_wrap_vnc_display_set_scaling, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "get_scaling", (PyCFunction)_wrap_vnc_display_get_scaling, METH_NOARGS,
      NULL },
    { "set_force_size", (PyCFunction)_wrap_vnc_display_set_force_size, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "get_force_size", (PyCFunction)_wrap_vnc_display_get_force_size, METH_NOARGS,
      NULL },
    { "set_shared_flag", (PyCFunction)_wrap_vnc_display_set_shared_flag, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "get_shared_flag", (PyCFunction)_wrap_vnc_display_get_shared_flag, METH_NOARGS,
      NULL },
    { "set_depth", (PyCFunction)_wrap_vnc_display_set_depth, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "get_depth", (PyCFunction)_wrap_vnc_display_get_depth, METH_NOARGS,
      NULL },
    { "force_grab", (PyCFunction)_wrap_vnc_display_force_grab, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "is_pointer_absolute", (PyCFunction)_wrap_vnc_display_is_pointer_absolute, METH_NOARGS,
      NULL },
    { "request_update", (PyCFunction)_wrap_vnc_display_request_update, METH_NOARGS,
      NULL },
    { NULL, NULL, 0, NULL }
};

PyTypeObject G_GNUC_INTERNAL PyVncDisplay_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                                 /* ob_size */
    "gtkvnc.Display",                   /* tp_name */
    sizeof(PyGObject),          /* tp_basicsize */
    0,                                 /* tp_itemsize */
    /* methods */
    (destructor)0,        /* tp_dealloc */
    (printfunc)0,                      /* tp_print */
    (getattrfunc)0,       /* tp_getattr */
    (setattrfunc)0,       /* tp_setattr */
    (cmpfunc)0,           /* tp_compare */
    (reprfunc)0,             /* tp_repr */
    (PyNumberMethods*)0,     /* tp_as_number */
    (PySequenceMethods*)0, /* tp_as_sequence */
    (PyMappingMethods*)0,   /* tp_as_mapping */
    (hashfunc)0,             /* tp_hash */
    (ternaryfunc)0,          /* tp_call */
    (reprfunc)0,              /* tp_str */
    (getattrofunc)0,     /* tp_getattro */
    (setattrofunc)0,     /* tp_setattro */
    (PyBufferProcs*)0,  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                      /* tp_flags */
    NULL,                        /* Documentation string */
    (traverseproc)0,     /* tp_traverse */
    (inquiry)0,             /* tp_clear */
    (richcmpfunc)0,   /* tp_richcompare */
    offsetof(PyGObject, weakreflist),             /* tp_weaklistoffset */
    (getiterfunc)0,          /* tp_iter */
    (iternextfunc)0,     /* tp_iternext */
    (struct PyMethodDef*)_PyVncDisplay_methods, /* tp_methods */
    (struct PyMemberDef*)0,              /* tp_members */
    (struct PyGetSetDef*)0,  /* tp_getset */
    NULL,                              /* tp_base */
    NULL,                              /* tp_dict */
    (descrgetfunc)0,    /* tp_descr_get */
    (descrsetfunc)0,    /* tp_descr_set */
    offsetof(PyGObject, inst_dict),                 /* tp_dictoffset */
    (initproc)_wrap_vnc_display_new,             /* tp_init */
    (allocfunc)0,           /* tp_alloc */
    (newfunc)0,               /* tp_new */
    (freefunc)0,             /* tp_free */
    (inquiry)0              /* tp_is_gc */
};



/* ----------- functions ----------- */

const PyMethodDef gtkvnc_functions[] = {
    { NULL, NULL, 0, NULL }
};


/* ----------- enums and flags ----------- */

void
gtkvnc_add_constants(PyObject *module, const gchar *strip_prefix)
{
#ifdef VERSION
    PyModule_AddStringConstant(module, "__version__", VERSION);
#endif
  pyg_enum_add(module, "DisplayCredential", strip_prefix, VNC_TYPE_DISPLAY_CREDENTIAL);
  pyg_enum_add(module, "DisplayKeyEvent", strip_prefix, VNC_TYPE_DISPLAY_KEY_EVENT);
  pyg_enum_add(module, "DisplayDepthColor", strip_prefix, VNC_TYPE_DISPLAY_DEPTH_COLOR);

  if (PyErr_Occurred())
    PyErr_Print();
}

/* initialise stuff extension classes */
void
gtkvnc_register_classes(PyObject *d)
{
    PyObject *module;

    if ((module = PyImport_ImportModule("gtk")) != NULL) {
        _PyGtkDrawingArea_Type = (PyTypeObject *)PyObject_GetAttrString(module, "DrawingArea");
        if (_PyGtkDrawingArea_Type == NULL) {
            PyErr_SetString(PyExc_ImportError,
                "cannot import name DrawingArea from gtk");
            return ;
        }
    } else {
        PyErr_SetString(PyExc_ImportError,
            "could not import gtk");
        return ;
    }


#line 683 "vnc.c"
    pygobject_register_class(d, "VncDisplay", VNC_TYPE_DISPLAY, &PyVncDisplay_Type, Py_BuildValue("(O)", &PyGtkDrawingArea_Type));
    pyg_set_object_has_new_constructor(VNC_TYPE_DISPLAY);
}
