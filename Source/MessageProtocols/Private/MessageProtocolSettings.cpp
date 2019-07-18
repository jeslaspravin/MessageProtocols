// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageProtocolSettings.h"

float UMessageProtocolSettings::getSleepDuration()
{
	UMessageProtocolSettings* messageSettings=GetMutableDefault<UMessageProtocolSettings>();
	return messageSettings != nullptr?1.0f/FMath::Clamp((float)(messageSettings->maxFrequency), 1.0f, 60.0f):0.0f;
}
