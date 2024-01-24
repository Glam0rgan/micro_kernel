#pragma once

typedef enum api_object {
    osUntypedObject,
    osTCBObject,
    osEndpointObject,
    osNotificationObject,
    osCapTableObject,
    osNonArchObjectTypeCount,
}OSObjectType;

typedef uint64_t ApiObject;
