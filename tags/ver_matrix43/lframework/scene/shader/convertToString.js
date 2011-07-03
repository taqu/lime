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

var args = WScript.Arguments;

var infile;
var outfile = "out.txt";
if(WScript.Arguments.length>0){
    infile = WScript.Arguments(0);
}
if(WScript.Arguments.length>1){
    outfile = WScript.Arguments(1);
}

function convertToString(in_name, out_name)
{

    var inpath = fsys.BuildPath(scriptPath, in_name);
    var outpath = fsys.BuildPath(scriptPath, out_name);

    var ifile = fsys.OpenTextFile(inpath, Read);
    var ofile = fsys.OpenTextFile(outpath, Write, true);

    var line;
    var funcs = new Array();

    do{
        line = ifile.ReadLine();
        if(line.length>0){
            ofile.Write("\"" + line + "\\n\"\r\n");
        }
    }while(ifile.AtEndOfStream == false);

    ifile.Close();
    ofile.Close();
}

convertToString(infile, outfile);