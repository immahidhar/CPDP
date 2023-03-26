package edu.fsu.omp.repo;

import edu.fsu.omp.entity.Order;
import org.springframework.data.repository.CrudRepository;

import java.util.List;

public interface OrderRepository extends CrudRepository<Order, Integer> {
    List<Order> findByUserId(int userId);
    List<Order> findByProductId(int productId);
    void deleteAllByUserId(int userId);
}
