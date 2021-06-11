#include "../include/processing.h"
#include "Adder/adder.h"
#include "Filter/filter.h"
#include "Subtractor/subtractor.h"

//#define DEBUG_PROCESSING_MODULES

Processing::Processing(QObject *parent)
	: QObject(parent)
{
}

Processing *Processing::make(QString model)
{
	#ifdef DEBUG_PROCESSING_MODULES
		Logger->debug("Processing::make type: {}", model.toStdString());
	#endif

	if (model == "Filter")
	{
		return new Filter(nullptr);
	}
	else if (model == "Estimator")
	{
		return new Subtractor(nullptr);
	}
	else if (model == "Adder")
	{
		return new Adder(nullptr);
	}
	return new Filter(nullptr);
}
