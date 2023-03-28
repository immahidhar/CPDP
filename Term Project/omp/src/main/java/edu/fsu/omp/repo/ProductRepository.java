package edu.fsu.omp.repo;

import edu.fsu.omp.entity.Product;
import org.springframework.data.repository.CrudRepository;

import java.util.List;

public interface ProductRepository extends CrudRepository<Product, Integer> {
    List<Product> findByName(String name);
    List<Product> findByCategory(String category);
    default void deleteByName(String name) {
        deleteAll(findByName(name));
    }
    default void deleteByCategory(String category) {
        deleteAll(findByCategory(category));
    }
}
