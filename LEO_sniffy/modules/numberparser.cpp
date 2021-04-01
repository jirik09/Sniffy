#include "numberparser.h"

NumberParser::NumberParser(QObject *parent) : QObject(parent)
{

}

qreal NumberParser::parse(QString text, bool &success)
{
    bool isParsed = false ;
    qreal mult = 1, result = qQNaN();
    QString parse, suffix = text.right(1);

    if(suffix == "u"){mult = 0.000001;parse = text.left(text.length()-1);}
    else if (suffix == "m"){mult = 0.001;parse = text.left(text.length()-1);}
    else if (suffix == "k"){mult = 1000;parse = text.left(text.length()-1);}
    else if (suffix == "M"){mult = 1000000;parse = text.left(text.length()-1);}
    else{parse = text;}

    if(parse.indexOf('u')!=-1){mult *=0.000001; parse.replace('u','.');}
    else if(parse.indexOf('m')!=-1){mult *=0.001; parse.replace('m','.');}
    else if(parse.indexOf('k')!=-1){mult *=1000; parse.replace('k','.');}
    else if(parse.indexOf('M')!=-1){mult *=1000000; parse.replace('M','.');}

    double tmpRead = QLocale().toDouble(parse,&isParsed); //try local decimal separator
    if(!isParsed){  //try . as a separator
        tmpRead = parse.toDouble(&isParsed);
    }

    if(isParsed){
        result = mult * tmpRead;
    }
    success = isParsed;

    return result;
}

qreal NumberParser::parse(QString text)
{
    bool success;
    return parse(text, success);
}
