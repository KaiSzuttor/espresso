include(unit_test)

unit_test(NAME central_potential_test SRC central_potential_test.cpp DEPENDS
          interactions)
unit_test(NAME lennard_jones_test SRC lennard_jones_test.cpp DEPENDS
          interactions)
