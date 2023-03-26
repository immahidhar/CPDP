package edu.fsu.omp.api;

import edu.fsu.omp.data.OrderDTO;
import edu.fsu.omp.data.Status;
import edu.fsu.omp.entity.Order;
import edu.fsu.omp.entity.Product;
import edu.fsu.omp.entity.User;
import edu.fsu.omp.repo.OrderRepository;
import edu.fsu.omp.repo.ProductRepository;
import edu.fsu.omp.repo.UserRepository;
import edu.fsu.omp.util.ModelMapperUtil;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;

import java.util.Collections;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;
import java.util.stream.StreamSupport;

@Slf4j
@Controller
@RequestMapping(path="/order")
public class OrderController {
    @Autowired
    private ModelMapperUtil modelMapper;
    @Autowired
    private OrderRepository orderRepository;
    @Autowired
    private UserRepository userRepository;
    @Autowired
    private ProductRepository productRepository;
    @GetMapping(path="/get_all")
    @ResponseStatus(HttpStatus.OK)
    public @ResponseBody List<OrderDTO> getAllOrders() {
        return StreamSupport.stream(orderRepository.findAll().spliterator(), false)
                .map(this::convertToDTO).collect(Collectors.toList());
    }
    @GetMapping(path="/get")
    @ResponseStatus(HttpStatus.OK)
    public @ResponseBody List<OrderDTO> getProduct(@RequestParam(required = false) String id,
                                                   @RequestParam(required = false) String userId,
                                                   @RequestParam(required = false) String productId) {
        if(id != null && Integer.parseInt(id) != 0)
            return Collections.singletonList(convertToDTO(orderRepository.findById(Integer.parseInt(id)).get()));
        else if(userId != null && Integer.parseInt(userId) != 0)
                return orderRepository.findByUserId(Integer.parseInt(userId)).stream()
                        .map(this::convertToDTO).collect(Collectors.toList());
        else if(productId != null && Integer.parseInt(productId) != 0)
            return orderRepository.findByProductId(Integer.parseInt(productId)).stream()
                    .map(this::convertToDTO).collect(Collectors.toList());
        return null;
    }
    @PostMapping(path="/place")
    public ResponseEntity<String> placeOrder(@RequestBody OrderDTO orderDTO) {
        Optional<User> user = userRepository.findById(orderDTO.getUserId());
        if(!user.isPresent())
            return ResponseEntity.status(HttpStatus.NOT_ACCEPTABLE).body("User not valid!");
        Optional<Product> product = productRepository.findById(orderDTO.getProductId());
        if(!product.isPresent())
            return ResponseEntity.status(HttpStatus.NOT_ACCEPTABLE).body("Product not available!");
        if(product.get().getQuantity() - orderDTO.getQuantity() < 0)
            return ResponseEntity.status(HttpStatus.NOT_ACCEPTABLE)
                    .body("Not enough quantity of product available in inventory!");
        product.get().setQuantity(product.get().getQuantity() - orderDTO.getQuantity());
        Order order = modelMapper.map(orderDTO, Order.class);
        order.setStatus(Status.ORDERED.toString());
        log.debug(String.valueOf(order));
        orderRepository.save(order);
        return ResponseEntity.status(HttpStatus.CREATED).body("Order placed successfully");
    }
    @PutMapping(path="/update")
    public ResponseEntity<String> updateOrder(@RequestBody OrderDTO orderDTO) {
        Optional<Order> order = orderRepository.findById(orderDTO.getId());
        if(!order.isPresent())
            return ResponseEntity.status(HttpStatus.NOT_ACCEPTABLE).body("Order not available to update!");
        Optional<User> user = userRepository.findById(orderDTO.getUserId());
        if(!user.isPresent())
            return ResponseEntity.status(HttpStatus.NOT_ACCEPTABLE).body("User not valid!");
        Optional<Product> product = productRepository.findById(orderDTO.getProductId());
        if(!product.isPresent())
            return ResponseEntity.status(HttpStatus.NOT_ACCEPTABLE).body("Product not available!");
        if(product.get().getQuantity() + order.get().getQuantity() - orderDTO.getQuantity() < 0)
            return ResponseEntity.status(HttpStatus.NOT_ACCEPTABLE)
                    .body("Not enough quantity of product available in inventory!");
        order.get().setAddress(orderDTO.getAddress());
        order.get().setQuantity(orderDTO.getQuantity());
        order.get().setStatus(orderDTO.getStatus().toString());
        orderRepository.save(order.get());
        return ResponseEntity.status(HttpStatus.ACCEPTED).body("Order updated successfully!");
    }
    @DeleteMapping(path="/delete")
    public ResponseEntity<String> deleteOrder(@RequestParam(required = false) String id,
                                              @RequestParam(required = false) String userId) {
        if(id != null && Integer.parseInt(id) != 0) orderRepository.deleteById(Integer.parseInt(id));
        else if(userId != null && Integer.parseInt(userId) != 0)
            orderRepository.deleteAllByUserId(Integer.parseInt(userId));
        return ResponseEntity.status(HttpStatus.ACCEPTED).body("Product deleted!");
    }
    @DeleteMapping(path="/delete_all")
    @ResponseStatus(HttpStatus.ACCEPTED)
    public @ResponseBody String deleteAllOrders() {
        log.info("Deleting all orders from database!!!");
        orderRepository.deleteAll();
        return "All orders deleted!!!";
    }
    private OrderDTO convertToDTO(Order order) {return modelMapper.map(order, OrderDTO.class);}
}
