// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageProtocolSettings.h"

float UMessageProtocolSettings::getSleepDuration()
{
	return 1.0f/FMath::Clamp((float)(GetMutableDefault<UMessageProtocolSettings>()->maxFrequency), 1.0f, 60.0f);
}
