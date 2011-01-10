var Func = function(ret, name, args)
{
    this.ret = ret;
    this.name = name;
    this.args = args;
};


var Read = 1;
var Write = 2;
var Append = 8;

var fsys = new ActiveXObject("Scripting.FileSystemObject");
var scriptPath = String(WScript.ScriptFullName).replace(WScript.ScriptName, "");

function convertHeader(in_name, out_name)
{

    var inpath = fsys.BuildPath(scriptPath, in_name);
    var outpath = fsys.BuildPath(scriptPath, out_name);


    var ifile = fsys.OpenTextFile(inpath, Read);
    var ofile = fsys.OpenTextFile(outpath, Write, true);

    var line;
    var funcs = new Array();

    do{
        line = ifile.ReadLine();
        if(line.search("GL_APICALL") != -1){
            var pos = line.search(/\((\w|\s|,|\*|\[|\])*\);/);
            if(pos != -1){
                var func_args = line.substring(pos, line.length-1);
                var others = line.substring(0, pos);
                others = others.split(/\s+/);
                funcs.push( new Func(others[1], others[3], func_args) );
            }
        }
    }while(ifile.AtEndOfStream == false);

    for(var i=0; i<funcs.length; ++i){
        ofile.Write("LIME_ES2_FUNC(" + funcs[i].ret + ", " + funcs[i].name + ", " + funcs[i].args + ");\n");
    }

    ofile.Write("\n\n/**************************************************************/\n");
    for(var i=0; i<funcs.length; ++i){
        ofile.Write("#define " + funcs[i].name + "\t" + "LIME_ES2_FUNCPTR(" + funcs[i].name + ")\n");
    }

    ofile.Write("\n\n/**************************************************************/\n");
    for(var i=0; i<funcs.length; ++i){
        ofile.Write("LIM_ES2_GET_FUNCPTR(" + funcs[i].name + ");\n");
    }
    ifile.Close();
    ofile.Close();
}

convertHeader("gl2.h", "lgl2.h.txt");
convertHeader("gl2ext.h", "lgl2ext.h.txt");
