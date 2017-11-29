# DonerECS - A Tweaked Entity-Component System
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
**DonerECS doesn't pretend to be a cache friendly Entity-Component System.** It's not based on Systems either. In DonerECS each component has it's own Update method, and also others such as Init(), Activate(), Deactivate() and so on. The way of sharing information around is through messages. 
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

## TO BE CONTINUED