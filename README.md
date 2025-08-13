חגי לב הכהן - 322996059
עמית מדהלה - 211779582


GameManager rule choices that we made that are premitted (according to the forum):

1.
How the backward rule work:
When a tank performs a backward action, the tank afterwards has 2 chances to cancel it. It can cancel it ONLY with a forward movement.
Any other actions, will be ignored.
So for example, if a tank performs backward movement on step 1. Then, it will have a chance to cancel it with a forward movement, on either step 2 or step 3.
If it didn't cancel it on either step 2 or step 3, it will perform the backward movement on step 3.

2.
At each step, we first ask all the tanks for their actions. Then we move all the shells (destroy necessary objects in case of collisions), and then perform shell movements again and tank actions (and again, destroy necessary objects in case of collisions).

3.
What happens when two objects colide:
    If two or more objects (excpet walls) end up in the same square - then all the objects on that square are destroyed.
    If two objects are passing through each other (meaning they are meeting each other "between the squares") - then only they are destroyed.

4.
    If two tanks standing next to each other, shoot at each other - then only the bullets will be destroyed, and the tanks wouldn't (aligned with the second condition in clause 3).
