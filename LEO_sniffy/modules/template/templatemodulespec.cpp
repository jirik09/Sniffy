#include "templatemodulespec.h"

TemplateModuleSpec::TemplateModuleSpec(QObject *parent)
{
    Q_UNUSED(parent);
}

void TemplateModuleSpec::parseSpecification(QByteArray spec)
{
    QList<QByteArray> specParams = spec.split(':');
    foreach (QByteArray param, specParams){
        qDebug () << param;
    }

    //TODO parse spec into public variables
    //example:
    /*
    if(specParams.length()>=xx){
        QDataStream paramA_Stream(specParams[0]);
        paramA_Stream >> paramA_Variable;
    }
    */

}
