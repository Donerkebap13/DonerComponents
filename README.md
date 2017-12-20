# DonerECS - A Tweaked Entity-Component System
[![Build Status](https://travis-ci.org/Donerkebap13/DonerECS.svg?branch=master)](https://travis-ci.org/Donerkebap13/DonerECS) [![Build status](https://ci.appveyor.com/api/projects/status/3l2174mt6qm7627w/branch/master?svg=true)](https://ci.appveyor.com/project/Donerkebap13/donerecs/branch/master)

DonerECS, Doner Entity-Component System, or simply DECS, is a framework that uses C++11 features to provide a **Unity-like** entity-component System.

## Features
- Support for **complex entity hierarchies**, with parent/children relationships, activation, deactivation etc. 
- A **Handle System** to determine if any entity/component is still valid or has been destroyed already.
- Easy **component creation/registration** which can be easily added to your entities on the fly.
- **Messages** between entities, which are forwarded also to their children and their components.
- **Tags system** to add specific attributes to your entities.
- **Prefab system** to reuse and compose more complex hierarchies.
- A **JSON parsing system** to load your prefabs/entities/scenes from disk.
- **178 Unit Tests (and counting) ensures that everything should work as expected.**

## Disclaimer
**DonerECS doesn't pretend to be a cache friendly Entity-Component System.** It's not based on Systems either. In DonerECS each component has it's own `Update()` method, and also others such as `Init()`, `Activate()`, `Deactivate()` and so on. The way of sharing information around is through messages. 
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
[DonerECS_Asteroids_Example](https://github.com/Donerkebap13/DonerECS_Asteroids_Example)
An example project is **coming soon**. It'll be a simple Asteroids clone. My intention with that project is showing all the features the framework supports right now, such as:
- Entity hierarchy.
- Messaging between entities.
- Prefab system.
- Entity parsing from JSON.
- Component creation.
- Entity Tags.

## Tutorial
Here I'll try to illustrate the basic usage of the main systems of DonerECS. After reading this you'll have the basic knowledge on how things are organized and how they can be used. For a deeper understanding I recommend you to have a look to the [Example Project.](#example)

### Entities
`DonerECS::CEntity` is DonerECS's main actor. This class can contain different `DonerECS::CComponent` that defines its behavior. It also has information about its parent and its children. It can also receive POD messages and forward them to its components and its children. Last but not least, it can also be tagged.

Creating a new entity is as simple as:
```c++
#include <donerecs/entity/CEntityManager.h>

DonerECS::CEntityManager::CreateInstance();

DonerECS::CEntity *entity = DonerECS::CEntityManager::Get()->GetNewElement();
```
`DonerECS::CEntityManager::Get()->GetNewElement();` will return a valid `DonerECS::CEntity` as long as it hasn't run out of entities to generate. By default, DonerECS can have 4096 entities alive at the same time. This value is modifiable through the compiler flag `-DMAX_ENTITIES=4096` with a **maximum of  8.192 entities.**

#### Prefabs
DonerECS supports the definition of prefabs, so the user can define a specific entity hierarchy for reusing it wherever it's needed:
```c++
#include <donerecs/entity/CPrefabManager.h>

DonerECS::CPrefabManager::CreateInstance();

DonerECS::CPrefabManager::Get()->RegisterPrefab("prefabTest", anyEntityCreatedPreviously);
```

### Components
`DonerECS::CComponent` is the base class for any component in DonerECS. Components defines the entity's behavior by aggregation. They can listen to specific messages and perform actions accordingly. Any new component should inherit from this class and it can implement some basic methods, if needed. **The user can register up to 512 different components.**

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
To register this component in the system, so any entity can use it, we need to do the following:
```c++
#include <donerecs/component/CComponentFactoryManager.h>

DonerECS::CComponentFactoryManager::CreateInstance();

static constexpr int amountOfFooComponentsAvailable = 512;
ADD_COMPONENT_FACTORY("foo", CCompFoo, amountOfFooComponentsAvailable);
```
After creating the `DonerECS::CComponentFactoryManager` we can start registering our components into the system using the macro `ADD_COMPONENT_FACTORY`. The string it receives is to identify the component while parsing our entities from a **JSON file.** The last parameters is how many components will be available. As with the entities, **there's a maximum of  8.192 components** of the same kind alive at the same time.

#### Adding a Component to an Entity
Once a componet is registered into the system, it can be added to an entity in two different ways:
```c++
DonerECS::CComponent* component = entity->AddComponent<CCompFoo>();
// same as
CCompFoo* component = entity->AddComponent<CCompFoo>();
// or
DonerECS::CComponent* component = entity->AddComponent("foo");
// same as
CCompFoo* component = entity->AddComponent("foo");
```

#### Updating your Components
In DonerECS, components are updated by type, one type at a time, in the order they were registered into the system.
So in the example:
```c++
ADD_COMPONENT_FACTORY("foo", CCompFoo, 128);
ADD_COMPONENT_FACTORY("bar", CCompBar, 128);
```
All existing `CCompFoo` will be updated sequentially before updating all existing `CCompBar` components.

To do so, in you system's Update() loop, you should call:
```c++
DonerECS::CComponentFactoryManager::Get()->Update(yourDeltaTime);
```

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

void CCompFoo::OnDummyMessage(const SDummyMessage& message) {
	// ...
}

CCompFoo::RegisterMessages() {
	RegisterMessage(&CCompFoo::OnDummyMessage);
}
```
After registering the messages you want, you can start sending messages like this:
```c++
SDummyMessage message(2, 3);
entity->SendMessage(message); // this will propagate the message to all entity's components.
entity->SendMessageRecursive(message); // this will also propagate the message to all entity's children and their components.
```

### Handles
`DonerECS::CHandle` are a kind of **single thread smart pointers**. They point to a specific `DonerECS::CEntity` or `DonerECS::CComponent`, knowing at all moments if they're still valid or not or, in other words, if they've been destroyed somewhere else in the code.
**The size of a** `DonerECS::CHandle` **is 32 bits.**
The way of working in DonerECS is **we never store raw pointers** of `DonerECS::CEntity` or `DonerECS::CComponent`, we always store `DonerECS::CHandle`, so we can check if the element they point to is still valid, so we don't access dangling pointers. Any `DonerECS::CHandle` can be cast to a `DonerECS::CEntity` or `DonerECS::CComponent`. If the cast is valid and the element still exists, it'll return a valid pointer to the element. Otherwise it'll return `nullptr`.
Here's an example:
```c++
#include <donerecs/handle/CHandle.h>
#include <donerecs/entity/CEntityManager.h>

using namespace DonerECS;

CHandle entityHandle = CEntityManager::Get()->GetNewElement();

if (entityHandle) {
	// CEntityManager has return a valid CEntity
} else {
	// CEntityManager has run out of CEntities
}
CEntity* entity = entityHandle;
// entity will be valid as entityHandle points to an alive entity

CEntityManager::Get()->DestroyEntity(&entity);
// entity is nullptr at this point
// entityHandle == false as it points to a destroyed entity.
```
Also, you can send messages through handles. If the handle is valid, the message will be propagated properly. Otherwise, the message will be lost but nothing will crash:
```c++
DonerECS::CHandle handle = entity;
SDummyMessage message(2, 3);
handle.SendMessage(message);

```
### Tags
Tags are a way of adding more information to your entities, so then you can filter them, send messages only to entities with specific tags etc.
There are two ways of adding tags to the system, so you can use them later.
First one, declaring them directly in code:
```c++
#include <donerecs/tags/CTagsManager.h>

DonerECS::CTagsManager::CreateInstance();

DonerECS::CTagsManager::Get()->RegisterTag("Tag1");
DonerECS::CTagsManager::Get()->RegisterTag("TagN");
```
The second one, parsing them from a JSON file:
```c++
#include <donerecs/tags/CTagsManager.h>

DonerECS::CTagsManager::CreateInstance();

DonerECS::CTagsManager::Get()->ParseTagsFromFile("path/to/your/tags.json");
```
The format of the tags.json file is something similar to this:
```json
{ "tags": ["Tag1", "tag2", "tagN"] }
```

### Parsing a scene from a JSON file
DonerECS supports loading from disk using JSON (thanks to JSONCPP), so there's a way fo creating prefabs or scenes that can be stored as assets instead of building them from scratch in code every time we run our application.
The basic usage is as follows:
```c++
#include <donerecs/entities/CEntityParser.h>

DonerECS::CEntityParser parser;

CEntity* entity = parser.ParseSceneFromFile("path/to/your/scene.json");
```
The format of a scene.json file is something similar to this:
```json
{
	"type": "scene",
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
If, instead of parsing a scene we want to parse a **prefab** to register it automatically into `DonerECS::CPrefabManager`, we just need to change the `type` to `"prefab"`:
```json
{
	"type": "prefab",
	"root": { ... }
}
```
After doing this the prefab is available for any new parsed scene to use.

## TO BE CONTINUED
