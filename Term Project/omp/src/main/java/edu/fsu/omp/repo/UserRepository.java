package edu.fsu.omp.repo;

import edu.fsu.omp.entity.User;
import org.springframework.data.repository.CrudRepository;

import java.util.List;

public interface UserRepository extends CrudRepository<User, Integer> {
    List<User> findByUsername(String username);
    default void deleteByUsername(String username) {
        deleteAll(findByUsername(username));
    };
}
