![Doner Serializer](https://i.imgur.com/u80uptL.png)

[![Release version](https://img.shields.io/badge/release-v1.0.0-blue.svg)](https://github.com/Donerkebap13/DonerECS/releases/tag/1.0.0) [![Build Status](https://travis-ci.org/Donerkebap13/DonerECS.svg?branch=master)](https://travis-ci.org/Donerkebap13/DonerECS) [![Build status](https://ci.appveyor.com/api/projects/status/3l2174mt6qm7627w/branch/master?svg=true)](https://ci.appveyor.com/project/Donerkebap13/donerecs/branch/master) [![Coverage Status](https://coveralls.io/repos/github/Donerkebap13/DonerECS/badge.svg?branch=master&service=github)](https://coveralls.io/github/Donerkebap13/DonerECS?branch=master&service=github)

## DonerECS - A Tweaked GameObject-Component System

DonerECS, Doner GameObject-Component System, or simply DECS, is a framework that uses C++14 features to provide a **Unity-like** gameObject-component System.

## Features
- Support for **complex gameObject hierarchies**, with parent/children relationships, activation, deactivation etc. 
- A **Handle System** to determine if any gameObject/component is still valid or has been destroyed already.
- Easy **component creation/registration** which can be easily added to your entities on the fly.
- **Messages** between entities, which are forwarded also to their children and their components.
- **Tags system** to add specific attributes to your entities.
- **Prefab system** to reuse and compose more complex hierarchies. It supports **Nested Prefabs**.
- A **JSON parsing system** to load your prefabs/entities/scenes from disk.
- **200+ Unit Tests ensures that everything works as expected.**

## Disclaimer
**DonerECS doesn't pretend to be a cache friendly GameObject-Component System.** It's not based on Systems either. In DonerECS each component has it's own `Update()` method, and also others such as `Init()`, `Activate()`, `Deactivate()` and so on. The way of sharing information around is through messages. 
If you are not familiar with this way of working, I recommend you to have a look to the [Example Project](#example) or the  [Tutorial](#tutorial)  and give it a try!

## Downloading

You can acquire stable releases [here](https://github.com/Donerkebap13/DonerECS/releases).

Alternatively, you can check out the current development version with:

```
git clone https://github.com/Donerkebap13/DonerECS.git
```

## Contact

You can contact me directly via [email](mailto:donerkebap13@gmail.com)
Also, if you have any suggestion or you find any bug, please don't hesitate to [create a new Issue.](https://github.com/Donerkebap13/DonerECS/issues)

## Example
[DonerECS_Asteroids_Example](https://github.com/Donerkebap13/DonerECS_Asteroids_Example) is an **example project** I've created in order to show how to use **DonerECS**. It's a really simple Asteroids-wannabe clone. My intention with that project is to show all the features the framework supports right now, such as:
- GameObject hierarchy
- Messaging between entities
- Prefab system
- GameObject parsing from JSON
- Component creation
- GameObject Tags

## Tutorial
Here I'll try to illustrate the basic usage of the main systems of DonerECS. After reading this you'll have the basic knowledge on how things are organized and how they can be used. For a deeper understanding I recommend you to have a look to the [Example Project.](#example)

### Initialization
``CDonerECSSystems`` is a **singleton**that initializes and gives access to all **DonerECS** different systems.
It should be initialized:
 ```c++
#include <donerecs/CDonerECSSystems.h>

DonerECS::CDonerECSSystems::CreateInstance();
DonerECS::CDonerECSSystems::Get()->Init();
```
Updated:
 ```c++
float elapsed = ...;
DonerECS::CDonerECSSystems::Get()->Update(elapsed);
```
And destroyed:
 ```c++
DonerECS::CDonerECSSystems::DestroyInstance();
```

### Entities
`DonerECS::CGameObject` is DonerECS's main actor. This class can contain different `DonerECS::CComponent` that defines its behavior. It also has information about its parent and its children. It can also receive POD messages and forward them to its components and its children. Last but not least, it can also be tagged.

Creating a new gameObject is as simple as:
```c++
#include <donerecs/gameObject/CGameObject.h>

DonerECS::CGameObjectManager* gameObjectManager = DonerECS::CDonerECSSystems::Get()->GetGameObjectManager();
DonerECS::CGameObject *gameObject = gameObjectManager->GetNewElement();
```
`GetNewElement();` will return a valid `DonerECS::CGameObject` as long as it hasn't run out of entities to generate. By default, DonerECS can have 4096 entities alive at the same time. This value is modifiable through the compiler flag `-DMAX_ENTITIES=4096` with a **maximum of  8.192 entities.**

#### Prefabs
DonerECS supports the definition of prefabs, so the user can define a specific gameObject hierarchy for reusing it wherever it's needed:
```c++
#include <donerecs/gameObject/CPrefabManager.h>

DonerECS::CGameObjectManager* prefabManager = DonerECS::CDonerECSSystems::Get()->GetPrefabManager();
prefabManager->RegisterPrefab("prefabName", anyGameObjectCreatedPreviously);
```
**Prefabs** could be also loaded from a [JSON file](https://github.com/Donerkebap13/DonerECS/tree/feature/DonerECS-asteroids-development#parsing-a-prefab).

### Components
`DonerECS::CComponent` is the base class for any component in DonerECS. Components defines the gameObject's behavior by aggregation. They can listen to specific messages and perform actions accordingly. Any new component should inherit from this class and it can implement some basic methods, if needed. **The user can register up to 512 different components.**

Here's an example of a new component creation. **The implementation of all its methods is optional**:
```c++
#include <donerecs/component/CComponent.h>

class CCompFoo : public DonerECS::CComponent
{
public:
	void DoInit() override { m_foo = 0.0f; }
	void DoUpdate(float dt) override { m_foo += dt; }
	void DoDestroy() override { m_foo = 0.0f; }
	void DoActivate() override { }
	void DoDeactivate() override { }
private:
	float m_foo;
};
```
To register this component in the system, so any gameObject can use it, we need to do the following:
```c++
#include <donerecs/component/CComponentFactoryManager.h>

static constexpr int amountOfFooComponentsAvailable = 512;
ADD_COMPONENT_FACTORY("foo", CCompFoo, amountOfFooComponentsAvailable);
```
After initializing the `DonerECS::CDonerECSSystems` we can start registering our components into the system using the macro `ADD_COMPONENT_FACTORY`. The string it receives is to identify the component while [parsing our entities from a **JSON file**](https://github.com/Donerkebap13/DonerECS/tree/feature/DonerECS-asteroids-development#parsing-a-scene-from-a-json-file). The last parameters is how many components will be available. As with the entities, **there's a maximum of  8.192 components** of the same kind alive at the same time.

#### Adding a Component to an GameObject
Once a componet is registered into the system, it can be added to an gameObject in two different ways:
```c++
DonerECS::CComponent* component = gameObject->AddComponent<CCompFoo>();
// same as
CCompFoo* component = gameObject->AddComponent<CCompFoo>();
// or
DonerECS::CComponent* component = gameObject->AddComponent("foo");
// same as
CCompFoo* component = gameObject->AddComponent("foo");
```

#### Updating your Components
In **DonerECS**, components are updated by type, one type at a time, in the order they were registered into the system.
So in the example:
```c++
ADD_COMPONENT_FACTORY("foo", CCompFoo, 128);
ADD_COMPONENT_FACTORY("bar", CCompBar, 128);
```
All existing `CCompFoo` will be updated sequentially before updating all existing `CCompBar` components.

#### Defining Serializable data for your components
You can define which data will be exposed to be modified in **JSON** using **[DonerSerializer](https://github.com/Donerkebap13/DonerSerializer)**. You can check [here](https://github.com/Donerkebap13/DonerSerializer#how-to-use-it) how to use it. In here I'm just going to show an example.
```c++
class CCompFoo : public DonerECS::CComponent
{
	DECS_DECLARE_COMPONENT_AS_SERIALIZABLE(CCompFoo)
public:
	CCompFoo();
private:
	float m_dummy1;
	std::vector<std::string> m_dummy2;
};

DONER_DEFINE_REFLECTION_DATA(CCompFoo,
	DONER_ADD_NAMED_VAR_INFO(m_dummy1, "dummy1"),
	DONER_ADD_NAMED_VAR_INFO(m_dummy2, "dummy2")
)
```
After exposing ``m_dummy1`` and ``m_dummy2``, we can define their values in **JSON** like this:
```json
{
	"root": {
		"name": "test1",
		"components": [
        	{
				"name": "foo",
				"dummy1": 1.0,
				"dummy2": ["Test1", "Test2", "Test3"]
			}
		]
	}
}
```
For a more in-depth look on how to read from **JSON** check **[this](https://github.com/Donerkebap13/DonerECS/tree/feature/DonerECS-asteroids-development#parsing-a-scene-from-a-json-file)**.

### Messages
DonerECS supports a message system to interact between different entities and components. **A message could be any Struct/Class defined by the user.** Usually it'll only contain data, no logic, but there's no limitation to this. 
This is how a `DonerECS::CComponent` can listen to a specific message:
```c++
// Somewhere in your code
struct SDummyMessage {
	SDummyMessage(int foo, int bar)
	 : m_foo(foo), m_bar(bar) {}
	 
	int m_foo = 0;
	int m_bar = 0;
}

// Inside your component
CCompFoo::RegisterMessages() {
	RegisterMessage(&CCompFoo::OnDummyMessage);
}

void CCompFoo::OnDummyMessage(const SDummyMessage& message) {
	// ...
}
```
After registering the messages you want, you can start sending messages like this:
```c++
SDummyMessage message(2, 3);
// This will propagate the message to all gameObject's components.
gameObject->SendMessage(message); 
// This will propagate the message to all gameObject's components and its children's components.
gameObject->SendMessage(message, DonerECS::ESendMessageType::Recursive); 
// This won't send the message to the current gameObject but it's children.
gameObject->SendMessageToChildren(message); 
// Same as before but recursively through all gameObject's children and children's children.
gameObject->SendMessageToChildren(message, DonerECS::ESendMessageType::Recursive); 
```
``SendMessage`` sends the message right away, in the same frame. If you want to delay sending the message until the end of the frame, use ``PostMessage`` instead.

Last but not least, if you want to send a message to **ALL** living entities, you can use ``BroadcastMessage``:
```c++
SDummyMessage message(2, 3);
// This will propagate the message to all entities alive.
gameObjectManager->BroadcastMessage(message); 
```

### Handles
`DonerECS::CHandle` are a kind of **single thread smart pointers**. They point to a specific `DonerECS::CGameObject` or `DonerECS::CComponent`, knowing at all moments if they're still valid or not or, in other words, if they've been destroyed somewhere else in the code.
**The size of a** `DonerECS::CHandle` **is 32 bits.**
The way of working in DonerECS is **we never store raw pointers** of `DonerECS::CGameObject` or `DonerECS::CComponent`, we always store `DonerECS::CHandle`, so we can check if the element they point to is still valid, so we don't access dangling pointers. Any `DonerECS::CHandle` can be cast to a `DonerECS::CGameObject` or `DonerECS::CComponent`. If the cast is valid and the element still exists, it'll return a valid pointer to the element. Otherwise it'll return `nullptr`.
Here's an example:
```c++
#include <donerecs/handle/CHandle.h>
#include <donerecs/gameObject/CGameObjectManager.h>

using namespace DonerECS;

CHandle gameObjectHandle = m_gameObjectManager->GetNewElement();

if (gameObjectHandle) {
	// CGameObjectManager has return a valid CGameObject
} else {
	// CGameObjectManager has run out of CEntities
}
CGameObject* gameObject = gameObjectHandle;
// gameObject will be valid as gameObjectHandle points to an alive gameObject

m_gameObjectManager->DestroyGameObject(&gameObject);
// gameObject is nullptr at this point
// gameObjectHandle == false as it points to a destroyed gameObject.
```
Also, you can send messages through handles. If the handle is valid, the message will be propagated properly. Otherwise, the message will be ignored:
```c++
DonerECS::CHandle handle = gameObject;
SDummyMessage message(2, 3);
handle.SendMessage(message);

```
### Tags
Tags are a way of adding more information to your entities, so then you can filter them, send messages only to entities with specific tags etc.
There are two ways of adding tags to the system, so you can use them later.
First one, declaring them directly in code:
```c++
#include <donerecs/tags/CTagsManager.h>

DonerECS::CTagsManager* tagsManager = DonerECS::CDonerECSSystems::Get()->GetTagsManager();
tagsManager->RegisterTag("Tag1");
tagsManager->RegisterTag("TagN");
```
The second one, parsing them from a JSON file:
```c++
tagsManager->ParseTagsFromFile("path/to/your/tags.json");
```
The format of the tags.json file is something similar to this:
```json
{ "tags": ["Tag1", "tag2", "tagN"] }
```

### Parsing a scene from a JSON file
DonerECS supports loading from disk using JSON thanks to **[DonerSerializer](https://github.com/Donerkebap13/DonerSerializer)**, so there's a way fo creating prefabs or scenes that can be stored as assets instead of building them from scratch in code every time we run our application.
The basic usage is as follows:
```c++
#include <donerecs/entities/CGameObjectParser.h>

DonerECS::CGameObjectParser parser;
CGameObject* gameObject = parser.ParseSceneFromFile("path/to/your/scene.json");
```
The format of a scene.json file is something similar to this:
```json
{
	"root": {
		"name": "test1",
		"tags": ["tag1", "tag2", "tag3"],
		"components": [
        	{
				"name": "comp_location",
				"x": 1,
				"y": -3,
				"z": 9
			},
			{
				"name": "comp_rotation",
				"radians": 0.2
			}
		],
		"children": [
        	{
              	"name": "test11",
              	"tags": ["tag1", "tag3"]
			}, 
            {
              	"name": "test12",
              	"initiallyActive":false
			}
        ]
	}
}
```
#### Parsing a prefab
If, instead of parsing a scene we want to parse a **prefab** to register it automatically into `DonerECS::CPrefabManager`, we just need to call ``ParsePrefabFromFile``:
```c++
#include <donerecs/entities/CGameObjectParser.h>

DonerECS::CGameObjectParser parser;
CGameObject* gameObject = parser.ParsePrefabFromFile("path/to/your/prefab.json");
```
After doing this the prefab is available for any new parsed scene to use.

#### Nested Prefabs
**DonerECS** supports prefabs that includes other prefabs, being able to override its component's information:

Prefab1.json
```json
{
	"root": {
		"name": "Prefab1",
		"components": [
        	{
				"name": "comp_location",
				"x": 1,
				"y": -3,
				"z": 9
			},
			{
				"name": "comp_rotation",
				"radians": 0.2
			},
			{
				"name": "sprite",
				"texture": "res/common/textures/asteroid_med.png"
			}
		]
	}
}
```
Prefab2.json
```json
{
	"root": {
		"name": "Prefab2",
		"prefab": "Prefab1",
		"components": [
        	{
				"name": "sprite",
				"texture": "res/common/textures/flower_big.png"
			}
		]
	}
}
```
Scene.json
```json
{
	"root": {
		"name": "root",
		"prefab": "Prefab2",
		"components": [
        	{
				"name": "comp_location",
				"x": 0,
				"y": 0,
				"z": 0
			}
		]
	}
}
```